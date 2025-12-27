		AREA    |.text|, CODE, READONLY, ALIGN=2
		EXPORT  conv3x3_operar_u8s8_ARM
		PRESERVE8
		ARM

conv3x3_operar_u8s8_ARM PROC
		STMDB   sp!, {r4-r8, r10, r11, lr}
		
		;r0=img, r1=filas, r2=cols, r3=k, [sp,#32]=out, [sp,#36]=norm_shift
		MOV r7, r0				;r7 = img
		MOV r5, r1				;r5 = filas
		MOV r4, r2				;r4 = cols
		MOV r8, r3				;r8 = k
		LDR r11, [sp, #32]		;r11 = img
		LDR r3, [sp, #36]		;r3 = norm_shift

		;Guarda 2 espacios de pila sp[0]=norm_shift y sp[4]=j
		SUB sp, sp, #8			;
		STR r3, [sp]			;guardamos para luego norm_shift

		;------------------ poner_bordes_a_cero ------------------
		;Usamos r0-r3,r6,r12 como temporales. 
		;No tocamos r4=cols, r5=filas, r7=img, r8=k, r10, r11=out.
		
		MOV r1, r11				;r1 = @out
		SUB r0, r5, #1			;r0 = filas - 1
		MLA r2, r0, r4, r11		;r2 = @out + (filas - 1)*cols
		MOV r3, r4				;c = cols
		MOV r0, #0				;r0 = 0
		
primer_for
		STRB r0, [r1], #1		;MemB[@out] = 0, r1=r1+1 (arriba)
		STRB r0, [r2], #1		;MemB[@out + r2] = 0, r2=r2+1 (abajo)
		SUBS r3, r3, #1			;c--, act.flags
		BNE primer_for
		
		CMP r5, #2				;si filas <= 2 salta a fors_fin
		BLE fors_fin			;

		SUB r12, r5, #1			;r12 = filas - 1
		SUB r6, r4, #1			;r6 = cols - 1
		MOV r3, #1				;r = r3 = 1
		
segundo_for
		MLA r1, r3, r4, r11		;r1 = @out + r*cols 
		STRB r0, [r1]			;MemB[@out + r*cols] = 0 (izda)
        STRB r0, [r1, r6]		;MemB[@out + r*cols + cols-1] = 0 (dcha)
		ADD r3, r3, #1			;r++
		CMP r3, r12				;si r < filas-1 salta a segundo_for
		BLT segundo_for			;
		
fors_fin
        ;---------------- FIN poner_bordes_a_cero ----------------

		MOV r10, #0				;nonzero = r10 = 0
		MOV r6, #1				;i = r6 = 1

for_i
		SUB r0, r5, #1			;r0 = filas - 1
		CMP r6, r0				;si i >= filas-1 salta a fin_for
		BGE fin_for				;

		MOV r1, #1				;j = r1 = 1

for_j
		SUB r0, r4, #1			;r0 = cols - 1
		CMP r1, r0				;si j >= cols-1 salta a siguiente_i
		BGE siguiente_i			;
		
        ;------------------ conv3x3_kernel_u8s8_C ------------------
		;Usamos r0-r3,r12 como temporales. 
        ;No tocamos r7=img, r4=cols, r8=k, [sp]=norm_shift, r6=i, r1=j
		
		STR r1, [sp, #4]		;salvamos j
        
		SUB r12, r6, #1			;r12 = i - 1
		MOV r3, r1				;r3 = j
		SUB r1, r3, #1			;r1 = j - 1
		MLA r12, r4, r12, r1	;r12 = (i-1)*cols + (j-1)
		ADD r12, r7, r12		;p = r12 = img + (i-1)*cols (j-1)

		MOV r0, r8				;k_cambiante = r0
	
		;r2 = acc
		;--- fila 0 ---
		LDRSB r3, [r0], #1		;r3 = k[0], r0 = r0 + 1
		LDRB r1, [r12], #1		;r1 = p[0], r12 = r12 +1
		MUL r2, r3, r1			;r2 = k[0]*p[0] 

        LDRSB r3, [r0], #1		;r3 = k[1], r0 = r0 + 1
		LDRB r1, [r12], #1		;r1 = p[1], r12 = r12 +1
		MLA r2, r3, r1, r2		;r2 = k[1]*p[1]+k[0]*p[0]


        LDRSB r3, [r0], #1		;r3 = k[2], r0 = r0 + 1
		LDRB r1, [r12], #1		;r1 = p[2], r12 = r12 +1
		MLA r2, r3, r1, r2		;r2 = k[2]*p[2]+k[1]*p[1]+k[0]*p[0]
		
        ADD r12, r12, r4		;p = p + cols
        SUB r12, r12, #3		;p = p + cols - 3 (porque avanzamos 3 en la fila)

        ;--- fila 1 ---
        LDRSB r3, [r0], #1		;r3 = k[3], r0 = r0 + 1
		LDRB r1, [r12], #1		;r1 = p[cols], r12 = r12 +1
		MLA r2, r3, r1, r2		;r2 = k[3]*p[cols]+k[2]*p[2]+k[1]*p[1]+k[0]*p[0]
        
		LDRSB r3, [r0], #1		;r3 = k[4], r0 = r0 + 1
		LDRB r1, [r12], #1		;r1 = p[cols+1], r12 = r12 +1
		MLA r2, r3, r1, r2		;r2 = k[4]*p[cols+1]+k[3]*p[cols]+...
		
		LDRSB r3, [r0], #1		;r3 = k[5], r0 = r0 + 1
		LDRB r1, [r12], #1		;r1 = p[cols+2], r12 = r12 +1
		MLA r2, r3, r1, r2		;r2 = k[5]*p[cols+2]+k[4]*p[cols+1]+*...
		
        ADD     r12, r12, r4	;p = p + cols 
        SUB     r12, r12, #3	;p = p + cols - 3 (porque avanzamos 3 en la fila)

        ;--- fila 2 ---
		LDRSB r3, [r0], #1		;r3 = k[6], r0 = r0 + 1
		LDRB r1, [r12], #1		;r1 = p[2*cols], r12 = r12 +1
		MLA r2, r3, r1, r2		;r2 = k[6]*p[2*cols]+k[5]*p[cols+2]+...
		
        LDRSB r3, [r0], #1		;r3 = k[7], r0 = r0 + 1
		LDRB r1, [r12], #1		;r1 = p[2*cols+1], r12 = r12 +1
		MLA r2, r3, r1, r2		;r2 = k[7]*p[2*cols+1]+k[6]*p[2*cols]+...
		
        LDRSB r3, [r0], #1		;r3 = k[8], r0 = r0 + 1
		LDRB r1, [r12], #1		;r1 = p[2*cols+2], r12 = r12 +1
		MLA r2, r3, r1, r2		;r2 = k[8]*p[2*cols+2]+k[7]*p[2*cols+1]+...
		
        MOV r0, r2				;r0 = acc
		LDR r3, [sp]			;r3 = norm_shift

		CMP r3, #0				;si norm_shift <=0 salta a fin_if
		BLE fin_if				;
		
		SUB r2, r3, #1			;r2 = norm_shift - 1
		MOV r1, #1				;r1 = 1
		MOV r1, r1, LSL r2		;bias = r1 = 1 << (norm_shift-1)

		CMP r0, #0				;si acc < 0 salta a else1
		BLT else1				;
		
if1
		ADD r1, r0, r1			;r1 = acc + bias
		MOV r0, r1, ASR r3		;r0 = acc = (acc + bias) << norm_shift
        B fin_if
else1
        SUB r1, r0, r1			;r1 = acc - bias
        MOV r0, r1, ASR r3		;r0 = acc = (acc - bias) << norm_shift
fin_if
		CMP r0, #0				;si acc >= 0 salta a if2
		BGE if2					;
        MOV r0, #0				;devolvemos 0
        B fin_if2
if2
        CMP r0, #255			;si acc <= 255 salta a fin_if2
        BLE fin_if2				;
        MOV r0, #255			;devolvemos 255
fin_if2
		;---------------- FIN conv3x3_kernel_u8s8_C ----------------

        ; restaurar j y escribir salida
        LDR r1, [sp, #4]        ;recuperamos j
		MLA r2, r6, r4, r1		;r2 = i*cols + j
		ADD r2, r11, r2			;r2 = @out + i*cols + j
		STRB r0, [r2]			;MemB[@out + i*cols + j] = y

        CMP r0, #0				;si y != 0 nonzero++		
        ADDNE r10, r10, #1		;

        ADD r1, r1, #1    		;j++
        B for_j

siguiente_i
        ADD r6, r6, #1         	;i++
        B for_i

fin_for
		MOV r0, r10				;return nonzero
		
		ADD sp, sp, #8			;liberamos la zona reservada a parámetros locales
		
		LDMIA sp!, {r4-r8, r10, r11, lr}
        BX lr
        ENDP

        END
