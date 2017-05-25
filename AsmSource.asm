.686
.MODEL FLAT, C
.STACK
.DATA
;-----------Local data------------------------------
intFormat   BYTE "x = 0x%08x y = 0x%08x k = 0x%08x", 10, 13, 0
.CODE
;-----------External usage--------------------------
EXTRN  printf : proc;// we'll use printf
;-----------Function definitions--------------------
speck_round_asm PROC ;speck_round_asm(uint32_t& x_, uint32_t& y_, uint32_t k_)
push ebp
mov ebp, esp ; 

mov eax, [ebp+8]; &x_
mov eax, [eax]; eax = x_

mov edx, [ebp+12]; &y_
mov edx, [edx]; edx = y ()

mov ecx, [ebp+16]; ecx = &k_
mov ecx, [ecx]; ebx = k_

;push ecx
;push ebx
;push eax
;lea edi, intFormat
;push edi
;call printf
;add esp,16


ROR eax, 8  ; x - „€„y„{„|. „ѓ„t„r„y„s „r„Ѓ„‚„p„r„Ђ „~„p „p„|„Ћ„†„p

ADD eax, edx ; x = x + y

XOR eax, ecx ; x = x XOR k

ROL edx, 3 ; y - „€„y„{„|. „ѓ„t„r„y„s „r„|„u„r„Ђ „~„p „q„u„„„p

XOR edx, eax ; y = x XOR y


mov edi, [esp+8]; &x_
mov [edi], eax; x_ = x

mov edi, [esp+12]; &y_
mov [edi], edx; y_ = y

pop ebp ;
ret

speck_round_asm ENDP

speck_round_dec_asm PROC ;speck_round_dec_asm(uint32_t& x_, uint32_t& y_, uint32_t k_)
push ebp
mov ebp, esp ; 

mov eax, [ebp+8]; &x_
mov eax, [eax]; eax = x_

mov edx, [ebp+12]; &y_
mov edx, [edx]; ebx = y

mov ecx, [ebp+16]; ecx = &k_
mov ecx, [ecx]; ecx = k

XOR edx, eax ; y = x XOR y
	
ROR edx, 3  ; y - „€„y„{„|. „ѓ„t„r„y„s „r„Ѓ„‚„p„r„Ђ „~„p „q„u„„„p

XOR eax, ecx ; x = x XOR k

SUB eax, edx ; x = x - y 

ROL eax, 8 ; x - „€„y„{„|. „ѓ„t„r„y„s „r„|„u„r„Ђ „~„p „p„|„Ћ„†„p

mov edi, [esp+8]; &x_
mov [edi], eax; x_ = x

mov edi, [esp+12]; &y_
mov [edi], edx; y_ = y

pop ebp ;
ret

speck_round_dec_asm ENDP

END
