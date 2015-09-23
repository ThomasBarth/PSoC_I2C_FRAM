/**
 * @file FRAM.c
 * @author  Thomas Barth <thomas@barth-dev.de>
 * @version 1.0
 */

/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include <project.h>
#include <stdlib.h>
#include "FRAM.h"

/*******************************************************************************
**                      Macros                                                **
*******************************************************************************/
#define I2C_API(F)          _CONCAT(I2C_INSTANCE,F)
#define _CONCAT(a,b)        CONCAT(a,b)
#define CONCAT(a,b)         a##b

#define FRAM_ADR_BYTES      2
#define FRAM_PS_SHIFT       15
#define FRAM_MSB_SHIFT      8
#define FRAM_PS_MASK        0x10000

/*******************************************************************************
**                      Locals                                                **
*******************************************************************************/
static uint32_t FRAM_current_adr=FRAM_INVALID_ADR;
static uint32_t FRAM_prep_adr(uint32_t adr, uint8_t * const adr_ary);

/*******************************************************************************
**                      Definitions                                           **
*******************************************************************************/
void FRAM_Start(void){ I2C_API(_Start();)}

uint32_t FRAM_get_adr(void){return FRAM_current_adr;}

uint8_t FRAM_get_slave_adr(void){return FRAM_SLAVE_ADR;}

uint32_t FRAM_I2C_Status(void){return I2C_API(_I2CMasterStatus();)};

uint32_t FRAM_set_adr(uint32_t adr, FRAM_wait_t wait){
        
    uint8_t adr_ary[FRAM_ADR_BYTES+1];
    uint32_t i2c_result;
    
    //check adress and prepare bytes
    if(FRAM_prep_adr(adr,adr_ary)!=FRAM_NO_ERROR)
        return FRAM_PARAMTER_ERROR;
    
    //set adr    
    i2c_result= I2C_API(_I2CMasterWriteBuf(adr_ary[FRAM_ADR_BYTES],adr_ary,FRAM_ADR_BYTES,I2C_API(_I2C_MODE_COMPLETE_XFER)));
    
    //wait for Master to complete previous transfer
    if(wait==FRAM_WAIT)
        while (0u == (I2C_API(_I2CMasterStatus()) & I2C_API(_I2C_MSTAT_WR_CMPLT)))   {/* busy wait */ }
    
    //if the I2C Operation succeeded: safe the set address as current
    if(!(i2c_result& I2C_API(_I2C_MSTR_NO_ERROR )))
        FRAM_current_adr=adr;
    
    //return result of I2C operation
    return i2c_result;
}

uint32_t FRAM_read_current_adr(uint8_t * const buffer, uint32_t count, FRAM_wait_t wait){
    
    uint32_t i2c_result;
    
    //check if parameters are valid
    if(buffer==NULL||count==0)
        return FRAM_PARAMTER_ERROR;
    
    //read from FRAM
    i2c_result=I2C_API(_I2CMasterReadBuf(FRAM_SLAVE_ADR,buffer,count,I2C_API(_I2C_MODE_COMPLETE_XFER) ));
    
    //if the operation was successfull, the internal address will be updated
    if(!(i2c_result& I2C_API(_I2C_MSTR_NO_ERROR )))
        FRAM_current_adr=(FRAM_current_adr+count)%FRAM_ADR_MAX;
    
    if(wait==FRAM_WAIT)
        while (0u == (I2C_API(_I2CMasterStatus()) & I2C_API(_I2C_MSTAT_RD_CMPLT)))   {/* busy wait */ }
    
    //return result of I2C operation
    return i2c_result;
}

uint32_t  FRAM_read_from_adr(uint32_t adr, uint8_t * const buffer, uint32_t count){

    uint32_t i2c_result;
    
    //check if we are maybe already at the right address
    if(FRAM_current_adr!=adr)
    {
        //set the address latch
        i2c_result=FRAM_set_adr(adr,FRAM_WAIT);

        //if there was an error, return
        if(i2c_result!= I2C_API(_I2C_MSTR_NO_ERROR ))
            return i2c_result;
    }
        
    //read the data
    return FRAM_read_current_adr(buffer,count,FRAM_WAIT);

}

uint32_t FRAM_write_to_adr(uint32_t adr, uint8_t * const buffer, uint32_t count){
    
    uint8_t adr_ary[FRAM_ADR_BYTES+1];
    uint32_t i2c_result;
    uint8_t* data_out;
    uint8_t i,j;
    
    //check if parameters are valid
    if(buffer==NULL||count==0)
        return FRAM_PARAMTER_ERROR;
    
    //check adress and prepare bytes
    if(FRAM_prep_adr(adr,adr_ary)!=FRAM_NO_ERROR)
        return FRAM_PARAMTER_ERROR;
    
    //allocate memory for output array
    data_out=malloc((count+FRAM_ADR_BYTES)*sizeof(uint8_t));
    
    //copy data into output array
    for(i=0;i<FRAM_ADR_BYTES;i++)
        data_out[i]=adr_ary[i];
    
    for(j=0;i<FRAM_ADR_BYTES+count;i++,j++)
        data_out[i]=buffer[j];
        
    //write to FRAM
    i2c_result= I2C_API(_I2CMasterWriteBuf(adr_ary[FRAM_ADR_BYTES],data_out,FRAM_ADR_BYTES+count,I2C_API(_I2C_MODE_COMPLETE_XFER)));
    
    //wait for Master to complete previous transfer
    while (0u == (I2C_API(_I2CMasterStatus()) & I2C_API(_I2C_MSTAT_WR_CMPLT)))   {/* busy wait */ }
    
    //if the I2C Operation succeeded: safe the set address as current
    if(!(i2c_result& I2C_API(_I2C_MSTR_NO_ERROR )))
        FRAM_current_adr=adr+count;
    
    return i2c_result;
}

static uint32_t FRAM_prep_adr(uint32_t adr, uint8_t * const adr_ary){
    
    //check if adress is in range
    if(adr>FRAM_ADR_MAX)
        return FRAM_PARAMTER_ERROR;
    
    //Address MSB
    adr_ary[0]=adr>>FRAM_MSB_SHIFT;
    
    //Address LSB
    adr_ary[1]=adr;
    
    //modify slave adr to include the Page Select (PS) bit
    adr_ary[2]=FRAM_SLAVE_ADR|((adr&FRAM_PS_MASK)>>FRAM_PS_SHIFT);
    
    return FRAM_NO_ERROR;
}

/* [] END OF FILE */
