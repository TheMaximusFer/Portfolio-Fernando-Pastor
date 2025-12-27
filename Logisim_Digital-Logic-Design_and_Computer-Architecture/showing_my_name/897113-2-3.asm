.data 127                                       ; // @127
tty_out         .rw 1                           ; //PANTALLA.

.data 83                                        ; // @83
nombre:         .ascii 16 "FERNANDO PASTOR."    ; char nombre[16] = {F,E,R,N,A,N,D,O, , P,A,S,T,O,R,.}
punto:          .ascii 16 '.'                   ; //Punto para la comparación del while.
cero:           .dw 0                           ; //Constante 0 para reiniciar pantalla y para los bucles.
siguienteletra: .dw 128                         ; //Constante 128, en binario 10000000.
letra           .rw 1                           ; unsigned letra
minuscula_esp   .dw 32                          ; //Constante 32, para pasar a minúscula la mayúscula de ascii o para identificar el espacio que tiene el decimal 32 asignado en ascii. 
                                                ; i = 0

.code                                           ; // @0
                mov cero, tty_out               ; //Borra pantalla.
                mov nombre, letra               ; letra = nombre[i]
WHILE:                                          ; while(nombre[i] != '.'){        
                cmp letra, punto                
                beq HALT
                add minuscula_esp, letra        ; letra += 32 //Pasamos las letras a minúscula.
NORMAL:
                mov letra, tty_out              ; cout << letra
                add siguienteletra, NEXT        ; i++
NEXT:                           
                mov nombre, letra               ; //Se va a la siguiente letra del vector, ya que se suma 1 a @F.
                cmp minuscula_esp, letra        ; if(letra == ' '){
                beq NORMAL                      ; cout << letra }
                cmp cero, cero
                beq WHILE                       ; }
HALT:                                               
                cmp cero, cero
                beq HALT

.end

