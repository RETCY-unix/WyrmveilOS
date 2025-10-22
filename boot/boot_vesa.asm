BITS 16
ORG 0x7C00

start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    
    mov [boot_drive], dl

    mov si, msg_loading
    call print_string

    ; Get VESA VBE info
    mov ax, 0x4F00
    mov di, vbe_info_block
    int 0x10
    
    cmp ax, 0x004F
    jne vesa_error
    
    ; Set VESA mode 1024x768x32 (mode 0x118 or 0x115)
    mov ax, 0x4F02
    mov bx, 0x4118          ; Mode 0x118 with LFB bit set (bit 14)
    int 0x10
    
    cmp ax, 0x004F
    jne vesa_error
    
    ; Get mode info to find framebuffer address
    mov ax, 0x4F01
    mov cx, 0x118
    mov di, mode_info_block
    int 0x10
    
    ; Store framebuffer address for kernel
    mov eax, [mode_info_block + 40]  ; Physical base pointer
    mov [framebuffer_addr], eax
    
    mov si, msg_vesa_ok
    call print_string

    ; Load kernel sectors
    mov ah, 0x02
    mov al, 50
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [boot_drive]
    mov bx, 0x1000
    int 0x13
    
    jc disk_error

    mov si, msg_success
    call print_string

    cli
    lgdt [gdt_descriptor]
    
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    jmp 0x08:protected_mode

vesa_error:
    mov si, msg_vesa_error
    call print_string
    jmp $

disk_error:
    mov si, msg_error
    call print_string
    jmp $

print_string:
    pusha
.loop:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp .loop
.done:
    popa
    ret

boot_drive db 0
framebuffer_addr dd 0

msg_loading db 'SEPPUKU OS - Initializing VESA...', 13, 10, 0
msg_vesa_ok db 'VESA mode set: 1024x768x32', 13, 10, 0
msg_success db 'Kernel loaded! Starting...', 13, 10, 0
msg_error db 'DISK ERROR!', 13, 10, 0
msg_vesa_error db 'VESA ERROR! Graphics mode not supported', 13, 10, 0

gdt_start:
    dd 0x0, 0x0
    dw 0xFFFF, 0x0
    db 0x0, 10011010b, 11001111b, 0x0
    dw 0xFFFF, 0x0
    db 0x0, 10010010b, 11001111b, 0x0
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

BITS 32
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000
    
    ; Pass framebuffer address to kernel (in a known location)
    mov eax, [framebuffer_addr]
    mov [0x5000], eax       ; Store at 0x5000 for kernel to read
    
    call 0x1000
    
    jmp $

; VBE Info Block
vbe_info_block:
    times 512 db 0

; Mode Info Block  
mode_info_block:
    times 256 db 0

times 510-($-$$) db 0
dw 0xAA55
