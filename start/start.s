				PRESERVE8
				THUMB
					
				AREA	RESET, DATA, READONLY
				
				EXPORT	__Vectors
				IMPORT	HardFault_Handler
				IMPORT	UsageFault_Handler
				IMPORT	SVC_Handler
				IMPORT	SysTick_Handler
				EXPORT 	SysTick_Handler_asm
				EXPORT	UsageFault_Handler_asm

__Vectors		DCD		0
				DCD		Reset_Handler				; Reset Handler
				DCD     0                ; NMI Handler
                DCD     HardFault_Handler        	; Hard Fault Handler
                DCD     0          ; MPU Fault Handler
                DCD     0           ; Bus Fault Handler
                DCD     UsageFault_Handler_asm         	; Usage Fault Handler
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     SVC_Handler                ; SVCall Handler
                DCD     0           ; Debug Monitor Handler
                DCD     0                          ; Reserved
                DCD     0             ; PendSV Handler
                DCD     SysTick_Handler_asm            ; SysTick Handler
					
			
				AREA	|.text|, CODE, READONLY
					
Reset_Handler	PROC
				IMPORT	mymain
				IMPORT	mymemcpy
				;IMPORT	|Image$$RW_IRAM1$$Base|		; DST
				;IMPORT	|Image$$RW_IRAM1$$Length|	; Length
				;IMPORT	|Load$$RW_IRAM1$$Base|		; SRC
				
				IMPORT	usart1_init
				IMPORT	UsageFaultInit
				IMPORT	SystemInit
				IMPORT 	SysTickInit
				IMPORT 	LedInit
					
				
				EXPORT	Reset_Handler				[WEAK]
				LDR		SP, =(0x20000000+0x5000)
					
				;LDR		R0, =|Image$$RW_IRAM1$$Base|
				;LDR		R1, =|Load$$RW_IRAM1$$Base|
				;LDR		R2, =|Image$$RW_IRAM1$$Length|
				
				;BL		mymemcpy
				;LDR 	R0, =SystemInit
				;BLX 	R0
				;LDR 	R0, =usart1_init
				;BLX		R0
				;LDR 	R0, =UsageFaultInit
				;BLX 	R0
				
				;LDR		R0, =0x11111111
				;LDR		R1, =0x22222222
				;LDR		R2, =0x33333333
				;LDR		R3, =0x44444444
				;LDR		R12, =0x55555555
				
				;SVC		#1
				
				;DCD		0xffffffff
				
				;LDR 	R0, =SysTickInit
				;BLX		R0
				BL 		SystemInit
				BL 		usart1_init
				BL 		UsageFaultInit
				BL		SysTickInit
				BL		LedInit
				
				B      	mymain
				
				ENDP
					
UsageFault_Handler_asm PROC
				MOV		R0, SP
				B 		UsageFault_Handler
				ENDP


SysTick_Handler_asm PROC
				; 保存现场
				IMPORT save_stack_frame
				IMPORT save_stack
				IMPORT |cur_task|
				
				LDR R0, =|cur_task|
				LDR R0, [R0, #0]
				;CBNZ R0, save_stack_asm
				CMP R0, #0
				;BNE	SAVE_OLD_STACK
				BEQ DO_NOT_SAVE_STACK
				
SAVE_OLD_STACK
				;PUSH {R4-R11}
				STMDB SP!, {R4-R11}
				LDR R0, =|cur_task|
				LDR R0, [R0, #0]
				STR SP, [R0, #0x1c]
				
				;BL 		save_stack
				
DO_NOT_SAVE_STACK
				
				; CMP 	cur_task, #0
				;MOV		R0, LR
				;MOV		R0,	#0xFFFFFFF9
				BL 		SysTick_Handler
				ENDP

save_stack_asm PROC
				IMPORT |cur_task|
				PUSH {R4-R11}
				LDR R0, =|cur_task|
				LDR R0, [R0, #0]
				STR SP, [R0, #0x1c]
				
				BX LR
				ENDP


start_task_asm			PROC
				EXPORT	start_task_asm
				LDMIA R0!, {R4-R11}

				MSR MSP, R0

				BX R1
				ENDP

				END
