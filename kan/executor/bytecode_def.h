//
// Created by kvxmmu on 2/18/21.
//

#ifndef KANLANG_BYTECODE_DEF_H
#define KANLANG_BYTECODE_DEF_H

#define PLUSB 1u
#define MINUSB 2u
#define MULB 3u
#define DIVB 4u
#define GET_ATTRIBUTEB 5u
#define POWB 6u
#define CALLB 7u
#define PUSH_INTEGRAL 8u
#define LOAD_ID 9u
#define PUSH_STRING 10u
#define UPLUS 11u
#define UMINUS 12u
#define SET_NAME 13u
#define START_SCOPE 14u
#define END_SCOPE 15u
#define GET_ATTRIBUTESB 16u
#define JMP_IF_FALSE 17u
#define JMP_IF_TRUE 18u
#define JMP_NEXT 19u
#define JMP_BACK 20u
#define POP_OBJECT 21u
#define EQUALB 22u
#define LESSERB 23u
#define GREATERB 24u
#define LESSER_OR_EQUALB 25u
#define GREATER_OR_EQUALB 26u
#define DEFINE_FUNCTION 27u
#define JMP_TO_ADDRESS 28u
#define RET 29u
#define RET_NULL 30u
#define PUSH_TRUE 31u
#define PUSH_FALSE 32u
#define CREATE_TYPE 33u
#define CREATE_METHOD 34u
#define GET_FROM_STACK_AND_ASSIGN 35u

// FORMAT:
/*
 * ASSIGN_VARIABLE:
 *
 */

#endif //KANLANG_BYTECODE_DEF_H
