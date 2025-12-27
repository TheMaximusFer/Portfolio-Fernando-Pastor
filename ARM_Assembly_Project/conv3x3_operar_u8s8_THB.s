        AREA    |.text|, CODE, READONLY, ALIGN=2
        EXPORT  conv3x3_operar_u8s8_THB
        PRESERVE8

        ARM
conv3x3_operar_u8s8_THB PROC
        STMDB   sp!, {r4-r8, r10, r11, lr}   ; 32 bytes

        ADR     r12, to_thb
        ADD     r12, r12, #1
        BX      r12

        THUMB
to_thb
		;r0=img, r1=filas, r2=cols, r3=k, [sp,#32]=out, [sp,#36]=norm_shift
		MOVS r5, r1             ;r5 = filas
		MOVS r4, r2             ;r4 = cols
		LDR r7, [sp, #32]       ;r7 = out
		LDR r1, [sp, #36]       ;r1 = norm_shift

		;Guarda 7 espacios de pila sp[0]=norm_shift, sp[4]=j, sp[8]=nonzero, sp[12]=img, sp[16]=k, sp[20]=out y sp[24]=i
		SUB sp, sp, #28			;Creamos los 7 espacios en pila
		STR r1, [sp]			;sp[0]=norm_shift
		MOVS r1, #0
		STR r1, [sp, #8]		;sp[8]=nonzero = 0
		STR r0, [sp, #12]		;sp[12]=img
		STR r3, [sp, #16]		;sp[16]=k
		STR r7, [sp, #20]		;sp[20]=out

		;------------------ poner_bordes_a_cero ------------------
		SUBS r6, r5, #1			;r6 = filas-1
		MOVS r2, r6				;r2 = filas-1
		MULS r2, r4, r2         ;r2 = (filas-1)*cols
		ADDS r2, r7, r2   	    ;r2 = @out + (filas-1)*cols
		MOVS r3, r4             ;c = cols
		MOVS r0, #0				;r0 = 0
		
primer_for
		STRB r0, [r7]           ;MemB[@out] = 0 (arriba)
		ADDS r7, r7, #1			;r7=r7+1
		STRB r0, [r2]			;MemB[r2] = 0 (abajo)
		ADDS r2, r2, #1			;r2=r2+1
		
		SUBS r3, r3, #1			;c--, act.flags
		BNE primer_for

		CMP r5, #2				;si filas <= 2 salta a fors_fin
		BLE fors_fin			;
		
		MOVS r3, #1          	;r = r3 = 1
		SUBS r1, r4, #1     	;r1 = cols-1
		
segundo_for
		LDR r7, [sp, #20]       ;r7 = @out (el inicial)
		MOVS r2, r3				;r2 = r
		MULS r2, r4, r2  	    ;r2 = r*cols
		ADDS r7, r7, r2 		;r7 = @out + r*cols
		STRB r0, [r7]			;MemB[@out + r*cols] = 0 (izda)
		STRB r0, [r7, r1]		;MemB[@out + r*cols + cols-1] = 0 (dcha)
		ADDS r3, r3, #1			;r++
		
		CMP r3, r6				;si r < filas-1 salta a segundo_for		
		BLT segundo_for			;
fors_fin
		;---------------- FIN poner_bordes_a_cero ----------------
		;r1 = cols-1, r4= cols, r5 = filas, r6= filas-1, r0,r2,r3,r7
		MOVS r0, #1		        ;i = r0 = 1

for_i
		SUBS r3, r5, #1     	;r3 = filas-1
		CMP r0, r3				;si i >= filas-1 salta a fin_for
		BGE fin_for				;
		
		MOVS r2, #1         	;j = r2 = 1

for_j
		SUBS r3, r4, #1      	;r3 = cols-1
		CMP r2, r3				;si j >= cols-1 salta a siguiente_i
		BGE siguiente_i			;

;------------------ conv3x3_kernel_u8s8_C ------------------
;r0 = i, r1 = cols-1, r2 = j, r4= cols, r5 = filas, r6= filas-1,r3,r7
		STR r2, [sp, #4]         ;salvamos j
		STR r0, [sp, #24]        ;salvamos i

		; p = img + (i-1)*cols + (j-1)
		LDR r3, [sp, #12]   	;r3 = img
		SUBS r7, r0, #1      	;r7 = i - 1
		MULS r7, r4, r7      	;r7 = (i-1)*cols
		SUBS r2, r2, #1      	;r2 = j-1
		ADDS r7, r7, r2			;r7 = (i-1)*cols + (j-1) 			
		ADDS r7, r7, r3      	;p = r7 = img + (i-1)*cols + (j-1)

		LDR r3, [sp, #16]   	;k_cambiante = r3
		MOVS r0, #0          	;acc = r0
	
		MOVS r2, #0

        ;--- fila 0 ---
        LDRSB r6, [r3, r2]      ;r6 = k[0]
        ADDS r3, r3, #1			;r3=r3+1
        LDRB r1, [r7]           ;r1 = p[0]
        ADDS r7, r7, #1			;r7=r7+1
        MULS r1, r6, r1         ;r1 = k[0]*p[0] 
        ADDS r0, r0, r1			;r0 = k[0]*p[0] 

        LDRSB r6, [r3, r2]      ;r6 = k[1]
        ADDS r3, r3, #1			;r3=r3+1
        LDRB r1, [r7]           ;r1 = p[1]
        ADDS r7, r7, #1			;r7=r7+1
        MULS r1, r6, r1         ;r1 = k[1]*p[1] 
        ADDS r0, r0, r1			;r0 = k[0]*p[0]+k[1]*p[1] 

        LDRSB r6, [r3, r2]      ;r6 = k[2]
        ADDS r3, r3, #1			;r3=r3+1
        LDRB r1, [r7]           ;r1 = p[2]
        ADDS r7, r7, #1			;r7=r7+1
        MULS r1, r6, r1         ;r1 = k[2]*p[2] 
        ADDS r0, r0, r1			;r0 = k[2]*p[2]+k[1]*p[1]+k[0]*p[0]


        ADDS    r7, r7, r4		;p = p + cols
        SUBS    r7, r7, #3		;p = p + cols - 3 (porque avanzamos 3 en la fila)

        ;--- fila 1 ---
        LDRSB r6, [r3, r2]      ;r6 = k[3]
        ADDS r3, r3, #1			;r3=r3+1
        LDRB r1, [r7]           ;r1 = p[cols]
        ADDS r7, r7, #1			;r7=r7+1
        MULS r1, r6, r1         ;r1 = k[3]*p[cols] 
        ADDS r0, r0, r1			;r0 = k[3]*p[cols]+k[2]*p[2]+k[1]*p[1]+k[0]*p[0]

        LDRSB r6, [r3, r2]      ;r6 = k[4]
        ADDS r3, r3, #1			;r3=r3+1
        LDRB r1, [r7]           ;r1 = p[cols+1]
        ADDS r7, r7, #1			;r7=r7+1
        MULS r1, r6, r1         ;r1 = k[4]*p[cols+1] 
        ADDS r0, r0, r1			;r0 = k[4]*p[cols+1]+k[3]*p[cols]+...

        LDRSB r6, [r3, r2]      ;r6 = k[5]
        ADDS r3, r3, #1			;r3=r3+1
        LDRB r1, [r7]           ;r1 = p[cols+2]
        ADDS r7, r7, #1			;r7=r7+1
        MULS r1, r6, r1         ;r1 = k[5]*p[cols+2] 
        ADDS r0, r0, r1			;r0 = k[5]*p[cols+2]+k[4]*p[cols+1]+*...


        ADDS    r7, r7, r4		;p = p + cols 
        SUBS    r7, r7, #3		;p = p + cols - 3 (porque avanzamos 3 en la fila)

        ;--- fila 2 ---
        LDRSB r6, [r3, r2]      ;r6 = k[6]
        ADDS r3, r3, #1			;r3=r3+1
        LDRB r1, [r7]           ;r1 = p[2*cols]
        ADDS r7, r7, #1			;r7=r7+1
        MULS r1, r6, r1         ;r1 = k[6]*p[2*cols] 
        ADDS r0, r0, r1			;r0 = k[6]*p[2*cols]+k[5]*p[cols+2]+...
        
		LDRSB r6, [r3, r2]      ;r6 = k[7]
        ADDS r3, r3, #1			;r3=r3+1
        LDRB r1, [r7]           ;r1 = p[2*cols+1]
        ADDS r7, r7, #1			;r7=r7+1
        MULS r1, r6, r1         ;r1 = k[7]*p[2*cols+1] 
        ADDS r0, r0, r1			;r0 = k[7]*p[2*cols+1]+k[6]*p[2*cols]+...
        
		LDRSB r6, [r3, r2]      ;r6 = k[8]
        ADDS r3, r3, #1			;r3=r3+1
        LDRB r1, [r7]           ;r1 = p[2*cols+2]
        ADDS r7, r7, #1			;r7=r7+1
        MULS r1, r6, r1         ;r1 = k[7]*p[2*cols+2] 
        ADDS r0, r0, r1			;r0 = k[8]*p[2*cols+2]+k[7]*p[2*cols+1]+...
		
		
		LDR r3, [sp, #0]        ;recuperamos norm_shift
		CMP r3, #0				;si norm_shift <=0 salta a fin_if
		BLE     fin_if			;

		MOVS r1, #1             ;r1 = 1
		SUBS r2, r3, #1         ;r2 = norm_shift-1
		LSLS r1, r1, r2         ;bias = r1 = 1 << (norm_shift-1)

        CMP r0, #0				;si acc < 0 salta a else1
        BLT else1				;

if1
        ADDS r1, r0, r1			;r1 = acc + bias
		ASRS r1, r1, r3			;r1 = acc = (acc + bias) << norm_shift
        MOVS r0, r1				;r0 = acc
		B       fin_if

else1
        SUBS r1, r0, r1			;r1 = acc - bias
        ASRS r1, r1, r3			;r1 = acc = (acc - bias) << norm_shift
		MOVS r0, r1				;r0 = acc
fin_if
        
        CMP r0, #0				;si acc >= 0 salta a if2
        BGE if2					;			
        MOVS r0, #0				;devolvemos 0			
        B fin_if2

if2
        CMP r0, #255			;si acc <= 255 salta a fin_if2
        BLE fin_if2				;
        MOVS r0, #255			;devolvemos 255
fin_if2

		LDR r6, [sp, #24]   	;recuperamos i
        MOVS r3, r6				;r3 = i	
        MULS r3, r4, r3  	    ;r3 = i*cols
        LDR r2, [sp, #4]    	;recuperamos j
        ADDS r3, r3, r2      	;r3 = i*cols + j
        LDR r2, [sp, #20]   	;recuperamos out
        ADDS r2, r2, r3			;r2 = @out + i*cols + j
        STRB r0, [r2]           ;MemB[@out + i*cols + j] = y

        CMP r0, #0				;si y == 0 salta a
        BEQ no_suma				;
        LDR r2, [sp, #8]		;recuperamos nonzero
        ADDS r2, r2, #1			;nonzero++
        STR r2, [sp, #8]		;sp[8]=nonzero
no_suma
        LDR r2, [sp, #4]		;recuperamos j en r2
		LDR r0, [sp, #24]   	;recuperamos i en r0
        ADDS r2, r2, #1			;j++
        B for_j


siguiente_i
		ADDS r0, r0, #1			;i++
		B for_i

fin_for
		LDR r0, [sp, #8]   	; return nonzero
		ADD sp, sp, #28				

		LDR r1, =from_th
		BX r1

		ARM
from_th
		LDMIA sp!, {r4-r8, r10, r11, lr}
		BX lr

		ENDP

		END

