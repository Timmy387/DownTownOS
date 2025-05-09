; bootload.asm
; Margaret Black, 2007
; Kevin Angstadt, 2025
;
; This is a simple bootloader that loads and executes a kernel at sector 3

	bits 16
KSEG  	equ 0x1000		;kernel goes into memory at 0x10000
KSIZE	equ 9			;max sectors read at once
FIRST   equ 6			;sectors read first time
KSTART	equ 3		 	;kernel lives at sector 3 (makes room for map & dir)
SSEG	equ 0x2000		;stack segment
READS 	equ 5			;number of sectors being read in. This is up to 30720 bytes, just under start of my 32k heap in second half of DS.

	; boot loader starts at 0 in segment 0x7c00
	org 0h

	; let's put the kernel at KSEG:0
	; set up the segment registers
	mov ax,KSEG
	mov ds,ax
	mov es,ax
	mov ax,SSEG
	mov ss,ax
	
	; let's have the stack start at KSEG:fff0
	mov ax,0xfff0
	mov sp,ax
	mov bp,ax

	; read in the kernel from the disk

	mov     cl,KSTART+1     ; cl holds sector number
	mov     dh,0     		; dh holds head number - 0
	mov     ch,0    		; ch holds track number - 0
	mov     ah,2            ; absolute disk read
	mov     al,FIRST        ; read KSIZE sectors
	mov     dl,0            ; read from floppy disk A
	mov     bx,0			; read into 0 (in the segment)
	int     13h				; call BIOS disk read function


	mov 	cl,1
	mov		dh,1
	mov		ch,0
	mov 	ah,2
	mov 	al,KSIZE
	mov 	dl,0
	mov 	bx,FIRST*512
	int 	13h

xor si,si
loop:
	cmp si,READS 			;track how many reads we have done
	jge done
	inc si
	mov cl,1
	add ch,dh
	xor dh,1
	mov ah,2
	mov al,KSIZE
	xor dl,dl
	add bx,KSIZE*512
	int 13h
	jmp loop


done:
	; call the kernel
	jmp KSEG:0

  ; fill remaining bytes with 0
	times 510-($-$$) db 0

	; AA55 tells BIOS that this is a valid bootloader
	dw 0xAA55
