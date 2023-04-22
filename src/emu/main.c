#include <pico/stdlib.h>
#include <pico/unique_id.h>
#include <stdio.h>
#include <w5500.h>
#include <commandnet.h>

const uint LOX_SOLENOID = 26;
const uint ETH_SOLENOID = 27;

//------------Runtime Values and Behaviour------------
CMDNET_VAR(lox_period, 140000)
CMDNET_VAR(lox_duty_cycle, 50000)
CMDNET_VAR(eth_period, 140000)
CMDNET_VAR(eth_duty_cycle, 50000)
CMDNET_VAR(lox_run, 1)
CMDNET_VAR(eth_run, 1)

CMDNET_COMMAND(lox_on) {
  gpio_put(LOX_SOLENOID, 1);
  return CMD_SUCCESS;
}

CMDNET_COMMAND(lox_off) {
  gpio_put(LOX_SOLENOID, 0);
  return CMD_SUCCESS;
}

CMDNET_COMMAND(eth_on) {
  gpio_put(ETH_SOLENOID, 1);
  return CMD_SUCCESS;
}

CMDNET_COMMAND(eth_off) {
  gpio_put(ETH_SOLENOID, 0);
  return CMD_SUCCESS;
}

cmdnet_cmd_t cmds[] = {
    cmdnet_eth_on,
    cmdnet_eth_off,
    cmdnet_lox_on,
    cmdnet_lox_off,
};

cmdnet_var_t vars[] = {
    cmdnet_lox_period,     cmdnet_lox_duty_cycle, cmdnet_eth_period,
    cmdnet_eth_duty_cycle, cmdnet_lox_run,        cmdnet_eth_run,
};

SPI_DEVICE(w5500, spi0, 17);

//------------Network Constants------------
ip_t gateway     = {192, 168, 2, 1};
ip_t subnet_mask = {255, 255, 255, 0};
ip_t src_ip      = {192, 168, 2, 50};

int main() {
  //------------All Initialization------------
  stdio_init_all();

  spi_init(spi0, 5000);
  gpio_set_function(18, GPIO_FUNC_SPI);
  gpio_set_function(19, GPIO_FUNC_SPI);
  gpio_set_function(16, GPIO_FUNC_SPI);

  gpio_init(LOX_SOLENOID);
  gpio_init(ETH_SOLENOID);

  gpio_put(LOX_SOLENOID, 0);
  gpio_put(ETH_SOLENOID, 0);

  gpio_set_dir(LOX_SOLENOID, GPIO_OUT);
  gpio_set_dir(ETH_SOLENOID, GPIO_OUT);

  w5500_set(w5500);
  w5500_init(w5500);

  pico_unique_board_id_t board_id;
  pico_get_unique_board_id(&board_id);

  mac_t src_mac = {0x02,           board_id.id[3], board_id.id[4],
                   board_id.id[5], board_id.id[6], board_id.id[7]};

  //------------Poll Initialization Complete------------
  while (!stdio_usb_connected()) tight_loop_contents();
  while (!w5500_ready(w5500)) tight_loop_contents();

  for (int i = 0; i < 10; i++) printf("\n");

  //------------Main Program Begin------------

  printf("Program: %s\n", PICO_PROGRAM_NAME);
  printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);
  printf("\n=====Startup Information=====\n");
  printf("Took %llu µs\n", time_us_64());

  //------------Setup and Configuration------------
  // W5500 Configuration
  w5500_config(w5500, src_mac, src_ip, subnet_mask, gateway);
  ip_t ip;
  w5500_rw(w5500, W5500_COMMON, W5500_SIPR0, false, ip, sizeof(ip));
  printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", src_mac[0], src_mac[1],
         src_mac[2], src_mac[3], src_mac[4], src_mac[5]);
  printf("IP: %d.%d.%d.%d\n", src_ip[0], src_ip[1], src_ip[2], src_ip[3]);

  // Networking
  tcp_server_t _server;
  tcp_server_t* server = &_server;
  tcp_server_init(server, w5500, W5500_S0, 8080);

  // Protocol
  cmdnet_t _cmdnet;
  cmdnet_t* cmdnet = &_cmdnet;
  cmdnet_init(cmdnet, server, cmds, sizeof(cmds) / sizeof(cmds[0]), vars,
              sizeof(vars) / sizeof(vars[0]));

  int64_t eth_cycle_start = 0;
  int64_t lox_cycle_start = 0;

  while (true) {
    tcp_server_poll(server);

    if (tcp_server_connected(server)) {
      cmdnet_handle(cmdnet);
    }

    int64_t solenoid_time = time_us_64();
    int64_t on_time       = eth_period * eth_duty_cycle / 100000;

    if (eth_run) {
      if (eth_cycle_start + eth_period <= solenoid_time) {
        eth_cycle_start = solenoid_time;
      }

      if (eth_cycle_start + on_time <= solenoid_time) {
        gpio_put(ETH_SOLENOID, 0);
      } else {
        gpio_put(ETH_SOLENOID, 1);
      }
    } else {
      gpio_put(ETH_SOLENOID, 0);
    }

    solenoid_time = time_us_64();
    on_time       = lox_period * lox_duty_cycle / 100000;

    if (lox_run) {
      if (lox_cycle_start + lox_period <= solenoid_time) {
        lox_cycle_start = solenoid_time;
      }

      if (lox_cycle_start + on_time <= solenoid_time) {
        gpio_put(LOX_SOLENOID, 0);
      } else {
        gpio_put(LOX_SOLENOID, 1);
      }
    } else {
      gpio_put(LOX_SOLENOID, 0);
    }
  }
}