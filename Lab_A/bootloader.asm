[BITS 16] 		; Empiezo en instrucciones de 16 bits, Real mode
[ORG 0x7C00]	; La BIOS carga el boot sector en la posicion de memoria 0x7C00

reset_drive:
	xor ah, ah		; Pongo en 0 para el comando de reset
	int 13h			; Llamo a la interrupcion 13h 
	or ah, ah		; Si no hay error ah es 0, pasa por la ALU
	jnz reset_drive ; Si ah no es 0 intento de vuelta
	
	xor ax, ax
	mov es, ax		; Seteo ES = 0
	mov bx, 0x0600  ; Direccion de memoria del micro-kernel 00:600h
	
	mov ah, 02h     ; Comando para leer
	mov al, 0Eh     ; Cantidad de sectores = 14 = Bytes(Kernel.bin) / 512
	xor ch, ch		; Cilindro 0
	mov cl, 02h		; Sector 2
	xor dh, dh		; Cabezal 0
	int 13h
	or ah, ah		; Checkeo si no hay error pasando por la ALU
	jnz reset_drive	; Si hay error se intenta de nuevo
	
	cli             ; Deshabilitamos las interrupciones
	xor ax, ax      ; Seteo AX = 0. Como no se puede setear directamente DS en 0 hay q hacer este paso antes
	mov ds, ax      ; Copio AX a DS para setear a DS=0
	
	lgdt [gdt_desc] ; Cargo el gdt
	
					; Ahora hay que setear el bit 0 del registro CR0 para estar listo para el modo protegido
	mov eax, cr0    ; Copio CR0 a EAX. Se usa EAX ya que CR0 es de 32 bits
	or eax, 1		; Seteo el primer bit de EAX en 1
	mov cr0, eax    ; Copio EAX a CR0
	
	jmp 08h:clear_pipe ; salto al segmento de codigo 08h con offset clear_pipe. Salto far para limpiar el pipeline
	
[BITS 32]           ; Protected Mode, 32 bits	
clear_pipe:
	mov ax, 10h     ; Copio el identificador del segmento de datos en AX
	mov ds, ax      ; Copio 10h en el data segment register
	mov ss, ax      ; Copio 10h en el stack segment register
	mov esp, 6000h  ; Stack Pointer en 6000h

	jmp 08h:0x0600  ; Salto al main
	
; Global Table Descriptor
gdt:				; Direccion de la gdt

gdt_null:			; Segmento NULL
	dd 0
	dd 0
	
gdt_code:			; Segmento de codigo
	dw 0x0FFFF		; segment limiter, bits 0..15
	dw 0			; base address, bits 16..31
	db 0            ; base bits, 16..23
	db 10011010b    ; readable, nonconforming, code, privilege 0
	db 11001111b    ; 32 bits, Granularity = 4Kb
	db 0			; base bits, 24..31
	
gdt_data:			; Segmento de datos
	dw 0x0FFFF		; segment limiter, bits 0..15
	dw 0			; base address, bits 16..31
	db 0			; base bits, 16..23
	db 10010010b	; writable, expand down, data, privilege 0
	db 11001111b	; 32 bits, Granularity = 4Kb
	db 0			; base bits, 24..31
	
gdt_end:			; Para calcular el size de la gdt, direccion del fin de la gdt

gdt_desc:				; Descriptor de la gdt
	dw gdt_end - gdt	; Limit, size
	dd gdt				; Direccion de la gdt
	

times 510-($-$$) db 0        ; Fill up the file with zeros
    dw 0AA55h                ; Boot sector identifyer
		