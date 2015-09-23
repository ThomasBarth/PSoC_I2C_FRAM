/**
 * @file FRAM.h
 * @author  Thomas Barth <thomas@barth-dev.de>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * https://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This driver realises the communication between a Cypress PSoC and a Cypress I2C FRAM chip.
 * This driver is tested for the usage with a Cypress FM24V10 (http://www.cypress.com/file/41666/download) F-RAM memory chip, other EEPROM or FRAM chips might also be compatible.
 */

#if !defined(FRAM_H)
#define FRAM_H

/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include <stdint.h>

/*******************************************************************************
**                      Macros                                                **
*******************************************************************************/
#define I2C_INSTANCE            I2C                     //Name of the I2C Instance to be used
#define FRAM_SLAVE_ADR          0x50                    //I2C Slave address of the FRAM On the PSoC4 CY8CKIT-042-BLE Pioneer Kit the slave adress is 0x50. The user can change the Slave-Address by relocating R32/36 and R33/37.
#define FRAM_ADR_MAX            0x1ffff                 //the highest address of the FRAM

#define FRAM_INVALID_ADR        0xffffffff              //address given back by "FRAM_get_adr" if the value of the FRAM address latch is unknown to the driver.
#define FRAM_PARAMTER_ERROR     0x200u                  //indicates a parameter error of a function
#define FRAM_NO_ERROR           0                       //indicates that a function succeeded

/*******************************************************************************
**                      Typedefs                                              **
*******************************************************************************/
typedef enum {FRAM_WAIT, FRAM_DONT_WAIT} FRAM_wait_t;   //TODO

/*******************************************************************************
**                      Declarations                                          **
*******************************************************************************/
/**
Start the I2C instance

Calls the "Start" Function of the I2C instance given by I2C_INSTANCE

@param  void
@return void
*/
void        FRAM_Start(void);

/**
Gets the address the FRAM internaly is currently pointing to

The FRAM uses an internal latch to save the it is currently pointing to.
There is no way to read this internal latch.
This function returns the address that was calculated based on the called functions.
Note that this value might be corrupted if the FRAM is repowered or similar.
If you are unsure if the internal adress is valid, use "FRAM_set_adr" to set the address manually.

@param  void
@return the current address. Is FRAM_INVALID_ADR if the address could not be determined
*/
uint32_t    FRAM_get_adr(void);

/**
Get the I2C Slave ID of the FRAM

Returns the I2C Slave address of the FRAM defined in FRAM_SLAVE_ADR. On the PSoC4 CY8CKIT-042-BLE Pioneer Kit the slave adress is 0x50. 
The user can change the Slave-Address by relocating R32/36 and R33/37.

@param  void
@return the current I2C slave address of the FRAM.
*/
uint8_t     FRAM_get_slave_adr(void);

/**
Get the I2C Master Status

Returns the result of the function "_I2CMasterStatus" executed on the I2C instance I2C_INSTANCE

@param  void
@return result of "_I2CMasterStatus"
*/
uint32_t    FRAM_I2C_Status(void);

/**
Set the address the FRAM is pointing to

The FRAM has an internal address latch which determines the datacell that is written or read.
With this function the user can set the value of this address latch.
The information about the current address will be updated if the operation succeeded

@param adr the address to be set 
TODO
@return FRAM_PARAMTER_ERROR if the address is bigger than FRAM_ADR_MAX
        FRAM_NO_ERROR if the operation succeeded
        any other value is the output of "_I2CMasterWriteBuf" and indicates an error in the I2C module
*/
uint32_t    FRAM_set_adr(uint32_t adr, FRAM_wait_t wait);

/**
Reads data from the current address

The FRAM has an internal address latch which determines the datacell that is read.
With this function the user can read data based on the value of th address latch.
The user might want to use "FRAM_get_adr" to get the current address or use "FRAM_set_adr" to set it.
The information about the current address will be updated if the operation succeeded.

@param buffer pointer to the memory where the received data will be stored
@param count number of bytes to be read
TODO
@return FRAM_PARAMTER_ERROR if either the buffer points to NULL or the count is 0
        FRAM_NO_ERROR if the operation succeeded
        any other value is the output of "_I2CMasterReadBuf" and indicates an error in the I2C module
*/
uint32_t    FRAM_read_current_adr(uint8_t * const buffer, uint32_t count, FRAM_wait_t wait);

/**
Reads data from a given address

With this function the user can read a number of bytes at a given address.
The information about the current address will be updated if the operation succeeded.
This function relies on the address saved in the driver. 
If the calculated address of the internal address latch matches the given address, this funktion skipps "FRAM_set_adr" which results in a faster execution.
If the user can not rely on the calculated address, it is safer to first call FRAM_set_adr manually to make sure the driver-value and the value of the internal address latch match.

@param adr address to be read
@param buffer pointer to the memory where the received data will be stored
@param count number of bytes to be read
TODO
@return FRAM_PARAMTER_ERROR if either the buffer points to NULL, the count is 0 or the address is bigger than FRAM_ADR_MAX
        FRAM_NO_ERROR if the operation succeeded
        any other value is the output of "_I2CMasterReadBuf" ( if "FRAM_set_adr" is called internally, the output might also come from "_I2CMasterWriteBuf") and indicates an error in the I2C module.
*/
uint32_t    FRAM_read_from_adr(uint32_t adr, uint8_t * const buffer, uint32_t count);

/**
Writes data to a given address

With this function the user can write a number of bytes at a given address.

@param adr address to be written
@param buffer pointer to the memory where the data to be send is stored
@param count number of bytes to be written
TODO
@return FRAM_PARAMTER_ERROR if either the buffer points to NULL, the count is 0 or the address is bigger than FRAM_ADR_MAX
        FRAM_NO_ERROR if the operation succeeded
        any other value is the output of "_I2CMasterWriteBuf" and indicates an error in the I2C module.
*/
uint32_t    FRAM_write_to_adr(uint32_t adr, uint8_t * const buffer, uint32_t count);

#endif /* (FRAM_H) */

/* [] END OF FILE */
