global _inb
global _moveCursor
global _hideCursor
global _readInScreen
global _getCursorPos
global _getCursorRow
global _readTime
global _setTime
global _writeToDisk
global _readFromDisk
global _charToMouse



_inb: ; char inb(void)
    ; check if a key is in keyboard buffer
    mov ah, 1
    int 0x16
    jz nokey

    mov ah,0
    int 0x16
    ret

nokey:
    xor ax,ax
    ret




_moveCursor: ; void moveCursor(int row, int col)
    ; first make sure cursor is displaying as normal
    mov ah, 0x01
    mov cx, 0x0607
    int 0x10

    ; now move cursor
    push bp
    mov bp, sp
    mov ah, 0x02
    mov bx, [bp+4]
    mov dh, bl
    mov bx, [bp+6]
    mov dl, bl
    xor bx, bx
    mov bh, 0
    int 0x10
    pop bp
    ret




_hideCursor: ; void clearCursor(void)
    mov ah, 1
    mov cx, 0x2000
    int 0x10
    ret




_readInScreen: ; void readInScreen(unsigned short * buff)
    push bp
    mov bp, sp
    push ds
    push si
    push di
    push cx
    mov di, [bp+4] ; buff pointer
    mov si, 0x0000 ; offset
    mov cx, 2000 ; counter
    mov ax, 0xB800
    mov ds, ax

    rep movsw

    pop cx
    pop di
    pop si
    pop ds
    pop bp
    ret




_getCursorPos: ; short getCursorPos(void)
    mov ah,3
    mov bh,0
    int 0x10
    mov ax,dx
    ret




_getCursorRow: ; char getCursorRow(void)
    call _getCursorPos
    xor al,al
    shr ax,8
    ret




_readTime: ; void readTime(unsigned short * high, unsigned short * low)
    push bp
    mov bp, sp
    push si
    mov ah,0
    int 0x1A
    mov si,[bp+4]
    mov [si],cx
    mov si,[bp+6]
    mov [si],dx
    pop si
    pop bp
    ret



_setTime: ; void setTime(short hours, short minutes, short seconds)
    ret




_writeToDisk: ; void writeToDisk(inode * target, short head, short track, short sector)
    push bp
    mov bp,sp
    mov ax,0x1000
    mov es,ax
    mov ah,3
    mov al,1
    mov dl,0
    mov bx,[bp+4]   ;block
    mov dh,[bp+6]   ;head
    mov ch,[bp+8]   ;track
    mov cl,[bp+10]  ;sector
    int 0x13
    jc writeError

    mov ax,1
    pop bp
    ret

writeError:
    xor ax,ax
    pop bp
    ret




_readFromDisk: ; void readFromDisk(inode * target, short head, short track, short sector)
    push bp
    mov bp,sp
    mov ax,0x1000
    mov es,ax
    mov ah,2
    mov al,1
    mov dl,0
    mov bx,[bp+4]   ;block
    mov dh,[bp+6]   ;head
    mov ch,[bp+8]   ;track
    mov cl,[bp+10]  ;sector
    int 0x13
    jc readError

    mov ax,1
    pop bp
    ret

readError:
    xor ax,ax
    pop bp
    ret




_charToMouse: ; void charToMouse(char c, char color)
    push sp
    mov bp,sp
    xor cx,cx
    inc cx          ;write one character
    mov al,[bp+4]
    xor bx,bx       ;bh = 0 = page
    mov bl,[bp+6]
    mov ah,0x09
    int 0x10
    pop bp