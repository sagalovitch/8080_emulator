#include <stdint.h>

typedef struct ConditionCodes {    
    uint8_t    z:1;    // Zero flag
    uint8_t    s:1;    // Sign flag (if bit 7 is set, set sign flag)
    uint8_t    p:1;    // Parity : handled by a subroutine
    uint8_t    cy:1;    // Carry flag (if larger than 8 bits)
    uint8_t    ac:1;    // Auxillary carry used mostly for BCD (binary coded decimal) math : space invaders does not use
    uint8_t    pad:3;    
} ConditionCodes;    

typedef struct State8080 {   
    uint8_t    a;    
    uint8_t    b;    
    uint8_t    c;    
    uint8_t    d;    
    uint8_t    e;    
    uint8_t    h;    
    uint8_t    l;    
    uint16_t    sp;    
    uint16_t    pc;    
    uint8_t     *memory;    
    struct      ConditionCodes      cc;    
    uint8_t     int_enable;    
} State8080;

void UnimplementedInstruction(State8080* state)    
{    
//pc will have advanced one, so undo that    
printf ("Error: Unimplemented instruction\n");    
exit(1);    
}    

void Emulate8080Op(State8080* state)    
{    
    unsigned char *opcode = &state->memory[state->pc];    

    switch(*opcode)    
    {    
        case 0x00: break;                   //NOP is easy!    
        case 0x01:                          //LXI   B,word    
            state->c = opcode[1];    
            state->b = opcode[2];    
            state->pc += 2;                  //Advance 2 more bytes    
            break;    
        case 0x03:  // INX b
            
        case 0x41: state->b = state->c; break;    //MOV B,C    
        case 0x42: state->b = state->d; break;    //MOV B,D    
        case 0x43: state->b = state->e; break;    //MOV B,E    
        case 0x80:      //ADD B    
        {    
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->b;    
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            // Parity is handled by a subroutine    
            state->cc.p = Parity(answer & 0xff);    
            state->a = answer & 0xff;    
        }    
       //The code for ADD can be condensed like this    
        case 0x81:      //ADD C    
        {    
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->c;    
            state->cc.z = ((answer & 0xff) == 0);    
            state->cc.s = ((answer & 0x80) != 0);    
            state->cc.cy = (answer > 0xff);    
            state->cc.p = Parity(answer&0xff);    
            state->a = answer & 0xff;    
        }  
            case 0x86:      //ADD M    
        {    
            uint16_t offset = (state->h<<8) | (state->l);    
            uint16_t answer = (uint16_t) state->a + state->memory[offset];    
            state->cc.z = ((answer & 0xff) == 0);    
            state->cc.s = ((answer & 0x80) != 0);    
            state->cc.cy = (answer > 0xff);    
            state->cc.p = Parity(answer&0xff);    
            state->a = answer & 0xff;    
        }    

        case 0xC6:      //ADI byte    
        {    
            uint16_t answer = (uint16_t) state->a + (uint16_t) opcode[1];    
            state->cc.z = ((answer & 0xff) == 0);    
            state->cc.s = ((answer & 0x80) != 0);    
            state->cc.cy = (answer > 0xff);    
            state->cc.p = Parity(answer&0xff);    
            state->a = answer & 0xff;    
        }      
    }    

        
    state->pc+=1;    
}    
