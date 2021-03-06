/*
    State diagram representation of the microwave oven controller
    Author : Mina Mounir Farid Gendi
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
//five functions to represent the five states of the microwave oven
void Stopped_State();
void  Cooking_State();
void  SW1_Pause_State();
void SW3_Pause_State();
void Weight_Entry_State();
void  Time_Entry_State();
/* the global variables representing the state transitions.
 They are declared volatile to tell the compiler to keep them inside the memory
 keypad buttons A , B , C , D are represented by 4 variables
 Illegal_Weight = 1 if the user inserts an invalid weight
 SW1,2,3 represent the three switches
 time_left represents timer
*/
volatile uint8_t Keypad_A , Keypad_B, Keypad_C , Keypad_D , Illegal_Weight ,Illegal_Time, SW1 , SW2 , SW3 , time_left ;
//function pointer to point to current state function , initialy is the stopped state
void (*state_ptr)() = Stopped_State ;

int main()
{

    while(1)
    {
       state_ptr(); //call the current state
    }

    return 0;
}

void Stopped_State()
{
    if( Keypad_A == 1 && SW3 == 1) //if A is pressed and door is closed ,go to Cooking State
    {
        state_ptr = Cooking_State ;
        state_ptr() ;
    }

    else if( (Keypad_B == 1 || Keypad_C == 1) && SW3 == 1 ) //if B or C is pressed and door is closed ,go to weight entry state
    {
        state_ptr = Weight_Entry_State ;
        state_ptr() ;
    }

    else if( Keypad_D == 1 && SW3 == 1) //if D is pressed and door is closed ,go to time entry state
    {
        state_ptr = Time_Entry_State ;
        state_ptr() ;
    }

     else //otherwise , stay in the current state
     {
         state_ptr() ;
     }

}
void  Cooking_State()
{
    if(time_left == 0) //if time has finished go to stopped state
    {
        state_ptr = Stopped_State ;
        state_ptr() ;
    }

    else if(SW1 == 0  ) //if switch 1 is pressed ,  go to SW1 pause state
    {
        state_ptr = SW1_Pause_State ;
        state_ptr() ;
    }
    else if(SW 3 == 0 ) //if switch 3 is pressed (door open) ,  go to SW3 pause state
    {
        state_ptr = SW3_Pause_State ;
        state_ptr() ;
    }
    else //otherwise , stay in the current state
    {
        state_ptr() ;
    }

}

void  SW1_Pause_State()
{
    
    if(SW1 == 0 )  //if switch 1 is pressed ,  go to stopped state
    {
        state_ptr = Stopped_State;
        state_ptr() ;
    }
    else if( SW2 == 0 ) //if switch 2  is pressed   go to cooking state
    {
        state_ptr = Cooking_State;
        state_ptr() ;
    }
    else //otherwise , stay in the current state
     {
        state_ptr() ;
     }
}
void SW3_Pause_State()
{
     if(SW3 == 1) //if door is closed , go to cooking state 
     {
        state_ptr = Cooking_State;
        state_ptr() ;
     }
     else //otherwise , stay in the current state
     {
          state_ptr() ;
     }
}
void Weight_Entry_State()
{
    if(SW1 == 0) //if SW1 is pressed (interrupt) go to pause state
    {
        state_ptr = SW1_Pause_State();
        state_ptr();
    }
    else if(Illegal_Weight == 0 && SW3 == 1) //if the weight is entered correctly and door is closed , go to cooking state
    {
        state_ptr = Cooking_State ;
        state_ptr() ;
    }
    else //otherwise , stay in the current state
    {
       state_ptr() ;
    }

}

void  Time_Entry_State()
{
    if(SW1 == 0) //if SW1 is pressed (interrupt) go to pause state
    {
        state_ptr = SW1_Pause_State();
        state_ptr();
    }
     else if(Illegal_Time == 1 && SW2 == 0 && SW3 == 1 ) //if SW2 is pressed and door is closed , go to cooking state
    {
        state_ptr = Cooking_State ;
        state_ptr() ;
    }
    else //otherwise , stay in the current state
    {
        state_ptr() ;
    }
}
