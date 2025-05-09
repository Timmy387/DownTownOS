; kernel.asm
; Margaret Black, 2007
; Kevin Angstadt, 2025

; kernel.asm contains assembly functions that you can use in your kernel

	global _putInMemory
	global _initVideo

; void putInMemory (int segment, int address, char character)
_putInMemory:
  push bp
	mov bp,sp
	push ds
	mov ax,[bp+4]
	mov si,[bp+6]
	mov cl,[bp+8]
	mov ds,ax
	mov [si],cl
	pop ds
	pop bp
	ret

; void initVideo (void)
_initVideo:
	xor ax, ax
	mov al, 3 		; 80x25, 16 color
	int 10
	ret
