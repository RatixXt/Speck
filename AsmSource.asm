.686
.MODEL FLAT, C
.STACK
.DATA
.CODE
;-----------Function definitions--------------------
speck_round_asm PROC ;speck_round_asm(uint32_t& x_, uint32_t& y_, uint32_t k_)
push ebp
mov ebp, esp ; 

mov eax, [ebp+8]; &x_
mov eax, [eax]; eax = x_

mov ebx, [ebp+12]; &y_
mov ebx, [ebx]; ebx = y

mov ecx, [ebp+16]; ecx = k_

ROR eax, 8  ; x - „ˆ„y„{„|. „ƒ„t„r„y„s „r„„‚„p„r„€ „~„p „p„|„„†„p

ADD eax, ebx ; x = x + y

XOR eax, ecx ; x = x XOR k

ROL ebx, 3 ; y - „ˆ„y„{„|. „ƒ„t„r„y„s „r„|„u„r„€ „~„p „q„u„„„p

XOR ebx, eax ; y = x XOR y


mov edx, [esp+8]; &x_
mov [edx], eax; x_ = x

mov edx, [esp+12]; &y_
mov [edx], ebx; y_ = y

pop ebp ;
ret

speck_round_asm ENDP

speck_round_dec_asm PROC ;speck_round_dec_asm(uint32_t& x_, uint32_t& y_, uint32_t k_)
push ebp
mov ebp, esp ; 

mov eax, [ebp+8]; &x_
mov eax, [eax]; eax = x_

mov ebx, [ebp+12]; &y_
mov ebx, [ebx]; ebx = y

mov ecx, [ebp+16]; ecx = k_

XOR ebx, eax ; y = x XOR y
	
ROR ebx, 3  ; y - „ˆ„y„{„|. „ƒ„t„r„y„s „r„„‚„p„r„€ „~„p „q„u„„„p

XOR eax, ecx ; x = x XOR k

SUB eax, ebx ; x = x - y 

ROL eax, 8 ; x - „ˆ„y„{„|. „ƒ„t„r„y„s „r„|„u„r„€ „~„p „p„|„„†„p

mov edx, [esp+8]; &x_
mov [edx], eax; x_ = x

mov edx, [esp+12]; &y_
mov [edx], ebx; y_ = y

pop ebp ;
ret

speck_round_dec_asm ENDP

END