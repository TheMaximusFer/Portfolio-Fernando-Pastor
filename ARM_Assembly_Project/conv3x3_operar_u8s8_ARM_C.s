        AREA    |.text|, CODE, READONLY, ALIGN=2
        EXPORT  conv3x3_operar_u8s8_ARM_C
        EXPORT  poner_bordes_a_cero_ARM_C
		EXPORT  conv3x3_kernel_u8s8_ARM_C
        PRESERVE8
        ARM
		
conv3x3_kernel_u8s8_ARM_C PROC
		;r0 = img, r1 = cols, r2 = k, r3 = norm_shift
		STMDB sp!, {r4-r8, r10, r11, lr}	;prólogo

		MOV r4, r0				;r4 = img
		MOV r5, r1				;r5 = cols
		MOV r6, r2				;r6 = k
		MOV r7, r3				;r7 = norm_shift
		
		LDR r10, [sp, #32]		;r10 = i
		LDR r11, [sp, #36]		;r11 = j

		SUB r10, r10, #1		;r10 = i - 1
		SUB r11, r11, #1		;r11 = j - 1
		MLA r10, r5, r10, r11	;r10 = (i - 1) * cols + (j - 1)
		ADD r10, r4, r10		;r10 = p = img + (i - 1) * cols + (j - 1)
		MOV r8, r10				;r8 = p

								;r2 = acc
		LDRSB r0, [r6], #1		;r0 = k[0], r6 = r6 + 1
		LDRB r1, [r10], #1		;r1 = p[0], r10 = r10 +1
		MUL r2, r0, r1			;r2 = k[0]*p[0] 
			LDRSB r0, [r6], #1	;r0 = k[1], r6 = r6 + 1 
		LDRB r1, [r10], #1		;r1 = p[1], r10 = r10 + 1
		MLA r2, r0, r1, r2		;r2=k[1]*p[1]+k[0]*p[0]
		LDRSB r0, [r6], #1		;r0 = k[2], r6 = r6 + 1 
		LDRB r1, [r10], #1		;r1 = p[2], r10 = r10 + 1
		MLA r2, r0, r1, r2		;r2=k[2]*p[2]+k[1]*p[1]+k[0]*p[0]
		LDRSB r0, [r6], #1		;r0 = k[3], r6 = r6 + 1 
		ADD r10, r8, r5			;r10 = p + cols
		LDRB r1, [r10], #1		;r1 = p[cols]
		MLA r2, r0, r1, r2		;r2=k[3]*p[cols]+k[2]*p[2]+k[1]*p[1]+k[0]*p[0]
		LDRSB r0, [r6], #1		;r0 = k[4], r6 = r6 + 1 
		LDRB r1, [r10], #1		;r1 = p[cols+1]
		MLA r2, r0, r1, r2		;r2=k[4]*p[cols+1]+k[3]*p[cols]+...
		LDRSB r0, [r6], #1		;r0 = k[5], r6 = r6 + 1 
		LDRB r1, [r10], #1		;r1 = p[cols+2]
		MLA r2, r0, r1, r2		;r2=k[5]*p[cols+2]+k[4]*p[cols+1]+*...
		ADD r10, r8, r5, LSL #1	;r10=p+2*cols
		LDRSB r0, [r6], #1		;r0 = k[6], r6 = r6 + 1 
		LDRB r1, [r10], #1		;r1 = p[2*cols]
		MLA r2, r0, r1, r2		;r2=k[6]*p[2*cols]+k[5]*p[cols+2]+...
		LDRSB r0, [r6], #1		;r0 = k[7], r6 = r6 + 1 
		LDRB r1, [r10], #1		;r1 = p[2*cols+1]
		MLA r2, r0, r1, r2		;r2=k[7]*p[2*cols+1]+k[6]*p[2*cols]+...
		LDRSB r0, [r6], #1		;r0 = k[8], r6 = r6 + 1 
		LDRB r1, [r10], #1		;r1 = p[2*cols+2]
		MLA r2, r0, r1, r2		;r2=k[8]*p[2*cols+2]+k[7]*p[2*cols+1]+...
				
		MOV r0, r2				;r0 = acc

		CMP r7, #0				;si norm_shift <= 0 salta a fin_if
		BLE fin_if 				;

		SUB r2, r7, #1			;r2 = norm_shift-1
		MOV r1, #1				;r1 = 1
		MOV r1, r1, LSL r2		;r1 = bias = 1 << (norm_shift-1)
			
		CMP r0, #0				;si acc < 0 salta a else
		BLT else1
if1
		ADD r1, r0, r1			;r1 = acc + bias
		MOV r0, r1, ASR r7		;r0 = acc = (acc + bias) << norm_shift
		B fin_if
else1
		SUB r1, r0, r1			;r1 = acc - bias
		MOV r0, r1, ASR r7		;r0 = acc = (acc - bias) << norm_shift
fin_if
		
		CMP r0, #0				;si acc >= 0 salta a if2
		BGE if2					;
		MOV r0, #0				;devolvemos 0
		B fin_if2
if2	
		CMP r0, #255			;si acc <= 255 salta al final
		BLE fin_if2				;
		MOV r0, #255			;devolvemos 255
fin_if2
		
			
		;epílogo
		LDMIA r13!, {r4-r8, r10, r11, lr}
		BX lr
        ENDP


		
poner_bordes_a_cero_ARM_C PROC
        STMDB sp!, {r4-r8, r10, r11, lr}	;prólogo
		
        MOV r4, r0  	        ;r4 = out
        MOV r6, r1      		;r6 = filas
        MOV r5, r2         		;r5 = cols
		
        SUB r0, r6, #1			;r0 = filas -1
        MUL r11, r0, r5			;r11= (filas-1)*cols
        ADD r11, r4, r11		;r11= @out +(filas-1)*cols
		
        MOV r1, r4 	    	    ;r1 = @out
        MOV r2, r11 	       	;r2 = @out +(filas-1)*cols
        MOV r3, r5         		;c = cols
        MOV r0, #0				;r0 = 0
primer_for
        STRB r0, [r1], #1		;MemB[@out] = 0, r1=r1+1 (arriba)
        STRB r0, [r2], #1		;MemB[@out + r2] = 0, r2=r2+1 (abajo)
        SUBS r3, r3, #1			;c--, act. flags
        BNE primer_for			;si c != 0 salta a primer_for
		
        CMP r6, #2				;si filas <= 2 salta a fors_fin
        BLE fors_fin			;
		
        SUB r12, r6, #1			;r12= filas-1
        SUB r7, r5, #1			;r7 = cols-1
        MOV r8, #1				;r = r8 = 1
segundo_for
        MUL r2, r8, r5			;r2 = r*cols
        ADD r1, r4, r2			;r1 = @out + r*cols
        STRB r0, [r1]			;MemB[@out + r*cols] = 0 (izda)
        STRB r0, [r1, r7]		;MemB[@out + r*cols + cols-1] = 0 (dcha)
        ADD r8, r8, #1			;r++
        CMP r8, r12				;si r < filas-1 salta a segundo_for
        BLT segundo_for			;
fors_fin		
		;epílogo
        LDMIA sp!, {r4-r8, r10, r11, lr}
        BX lr
        ENDP




conv3x3_operar_u8s8_ARM_C PROC
        STMDB sp!, {r4-r8, r10, r11, lr}	;prólogo

        MOV r7, r0				;r7 = img
        MOV r5, r1				;r5 = filas
        MOV r4, r2				;r4 = cols
        MOV r8, r3				;r8 = k
        LDR r11, [sp, #32]		;r11= out

		;poner_bordes_a_cero(out, filas, cols)
        MOV r0, r11 			;out
        MOV r1, r5				;filas
        MOV r2, r4				;cols
        BL poner_bordes_a_cero_ARM_C

        MOV r10, #0				;r10 = nonzero = 0
        MOV r6,  #1				;r6 = i = 1

for_i
        SUB r0,  r5, #1			;r0 = filas -1
        CMP r6,  r0				;si i >= filas-1 saltar a fin_for
        BGE fin_for				;

        MOV r1,  #1				;r1 = j = 1

for_j
        SUB r0,  r4, #1			;r0 = cols -1
        CMP r1,  r0				;si j >= cols-1 salta a siguiente_i
        BGE siguiente_i			;

		;conv3x3_kernel_u8s8_C(img, cols, k, norm_shift, i, j)
        MOV r0, r7				;img
        MOV r2, r8				;k
        LDR r3, [sp, #36]		;norm_shift
        SUB sp, sp, #8			;dejamos espacios en la pila para 2 parámetros
        STR r6, [sp]			;i
        STR r1, [sp, #4]		;j
        MOV r1, r4				;cols
        BL conv3x3_kernel_u8s8_ARM_C	;Devuelve r0 = y
        LDR r1, [sp, #4]		;recuperamos j en r1
        ADD sp, sp, #8			; quitamos espacios dedicados a los 2 parámetros

        MUL r2, r6, r4			;r2 = i*cols
        ADD r2, r2, r1			;r2 = i*cols + j
        ADD r2, r11, r2			;r2 = @out + i*cols + j
        STRB r0, [r2]			;MemB[@out + i*cols + j] = y
        CMP r0, #0				;si y != 0 nonzero++
        ADDNE r10, r10, #1		;

        ADD r1, r1, #1			;j++
        B for_j

siguiente_i
        ADD r6, r6, #1			;i++
        B for_i

fin_for
        MOV r0, r10		 		;return nonzero
		
		;epílogo
        LDMIA sp!, {r4-r8, r10, r11, lr}
        BX lr
        ENDP
			
        END


