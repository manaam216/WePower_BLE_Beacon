#include <zephyr/kernel.h>
#include <string.h>
#include <zephyr/logging/log.h>

#include "app_types.h"
#include "fram.h"
#include "encrypt.h"
#include "accel.h"
#include "temp_pressure.h"
#include "config_commands.h"
#include "i2c_sensors.h"
#include "device_config.h"
#include "error_output.h"

#include "app_burn_energy.h"
#include "app_manuf_data.h"
#include "comparator.h"
#include "app_bt.h"
#include "app_cli.h"
#include "app_gpio.h"

LOG_MODULE_REGISTER(wepower);

#define FW_VERSION      "Firmware Version 4.1"
#define FW_BUILD_DATE   "Build Date 1/6/24 ..."

uint8_t u8Polarity = 0;

static struct k_work_delayable update_frame_work;

// task to run config_commands.c module
struct k_work process_command_task;

// used to tell if the pressure sensor was properly triggered
extern bool is_temp_pressure_sensor_triggered;

/**
 * @brief 20ms packet timer callback, sends work until max packets. Starts inter-event sleep if there is a next event.
 * 
 * @param timer_handler timer_handler for the callback 
 */
static void timer_event_handler(struct k_timer *timer_handler);

K_TIMER_DEFINE(timer_event, timer_event_handler, NULL);
SYS_INIT(init_we_power_board_gpios, POST_KERNEL, 0);

/**
 * @brief 20ms packet timer callback, sends work until max packets. Starts inter-event sleep if maximum packets for a single events have been sent
 * 
 * @param timer_handler timer_handler for the callback 
 */
static void timer_event_handler(struct k_timer *timer_handler)
{
    TX_Repeat_Counter++;
    LOG_PRINTK(">>> K Timer Event %d", TX_Repeat_Counter);

    set_CN1_6();       

    if (TX_Repeat_Counter <= fram_data.event_max_packets) // starts at 0, we send 1 if max is 1 by incrementing after the test.
    {
        manufacture_data[PAYLOAD_TX_REPEAT_COUNTER_INDEX] = TX_Repeat_Counter;
        k_work_submit(&start_advertising_work_item);
    }
    else 
    {
        LOG_INF(">>> Sent maximum packets");
        TX_Repeat_Counter = TX_REPEAT_COUNTER_DEFAULT_VALUE;
        clear_CN1_6();
        k_timer_stop(&timer_event);
		if (fram_data.sleep_between_events)
			k_work_schedule(&update_frame_work, K_MSEC(fram_data.sleep_between_events));
        else 
        {
            burn_the_energy();
        }
    }
}

/**
 * @brief Function to update the advertsising frame. This function is called after every certain time to update the
 * manufacture data buffer.
 * 
 * @param work Work item for the thread
 */
void update_frame_work_fn(struct k_work *work)
{
    update_manufacture_data();
    // Start event timer
    k_timer_start(&timer_event, K_MSEC(fram_data.packet_interval), K_MSEC(fram_data.packet_interval));
}


/**
 * @brief Main Application
 * 
 * @return int 
 */
int main(void)
{
    /**
     * @brief  VEXT10 by comparator to determine if we are config or business
     * 
     */

    uint8_t application_mode = get_application_mode();

    if (application_mode == true)
    {
        if (init_uart() != 0)
        {
            LOG_ERR("Init UART failed");
            indicate_error(ERROR_TYPE_UART);
        }
        else
        {
            LOG_INF("UART initialization successful, starting CLI");

            (void)init_command_line_interface();
            toggle_CN_1_4();

            LOG_INF("\rBLE Firmware - UART Link\n");
            LOG_INF("\r%s %s\n", FW_VERSION, FW_BUILD_DATE);

            // Reading FRAM.
            dump_fram(true);

            k_work_init(&process_command_task, (k_work_handler_t) process_command_fn); 
        }
    }
    else
    {
        disable_uart();

        /**
         * @brief VBULK10 by comparator to determine if we are running out of gas
         * 
         */
        init_comparator_2_vbulk();

        // configure the IMU,
        app_accel_config();

        // pressure sensor config.  
        enable_temp_pressure_sensor_interrupt_config ();
        
        // To save time later, start it early
        if (TEMP_PRESSURE_SUCCESS == app_temp_pressure_trigger())
        {
            is_temp_pressure_sensor_triggered = true;
        }
        else
        {
            is_temp_pressure_sensor_triggered = false;
        }
            
        /**
         * @brief Read FRAM and act accordingly.
         * 
         */
        if(dump_fram(true) == FRAM_SUCCESS)
        {
            k_sleep(K_MSEC(fram_data.sleep_after_wake)); 
            set_CN1_6();
            u8Polarity = read_polarity();
            clear_CN1_6();
        
            for(uint8_t i = 0; i < ENCRYPTED_KEY_NUM_BYTES; i++)
            {
                ecb_key[i] = fram_data.encrypted_key[i];
            }
        }
        else
        {
            // Reset event counter 
            fram_data.event_counter = 0;
            LOG_ERR("Failed to Read from FRAM Device");
            indicate_error(ERROR_TYPE_FRAM);
            burn_the_energy();
        }
    
        (void)initialize_bluetooth();	

        update_manufacture_data();

        LOG_INF("Starting K Worker Tasks");

        // Initialize a work item to trigger BLE advertising for each event
        k_work_init(&start_advertising_work_item, start_advertising_handler);

        // Initialize a delayable work item to update the advertising packet for every event by calling update_manufacture_data()
        k_work_init_delayable(&update_frame_work, update_frame_work_fn);

        // Trigger the 20ms timer to start advertising 
        k_timer_start(&timer_event, K_MSEC(fram_data.packet_interval), K_MSEC(fram_data.packet_interval));

        // so we don't wait 20ms, send first packet right now.
        toggle_CN_1_6();
        k_work_submit(&start_advertising_work_item);// submit the first packet to start advertising instantly
    }
        
    while (1) 
        k_msleep (10);  // supposedly this can end but ending main() seems wrong.
    return -1;
}
