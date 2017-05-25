// Speck 64/128 

#include <stdio.h>
#include <inttypes.h>
#include <bitset>
#include <iostream>

using namespace std;

#define ROUNDS 27

extern "C"
{
	void speck_round_asm(uint32_t& x_, uint32_t& y_, uint32_t& k_);
}

extern "C"
{
	void speck_round_dec_asm(uint32_t& x_, uint32_t& y_, uint32_t& k_);
}

static void speck_round(uint32_t& x_, uint32_t& y_, uint32_t& k_)
{
	uint8_t a = 8;
	uint8_t b = 3;

	uint32_t x = x_;
	uint32_t y = y_;
	uint32_t k = k_;
	//printf("0x%08x 0x%08x 0x%08x\n", x, y, k);

	__asm {
		
		MOV cl, a
		ROR x, cl  // x - „€„y„{„|. „ѓ„t„r„y„s „r„Ѓ„‚„p„r„Ђ „~„p „p„|„Ћ„†„p

		MOV eax, x  // x + y
		ADD eax, y

		XOR eax, k // x = x XOR k
		MOV x, eax
		
		MOV cl, b
		ROL y, cl // y - „€„y„{„|. „ѓ„t„r„y„s „r„|„u„r„Ђ „~„p „q„u„„„p

		XOR eax, y // y = x XOR y
		MOV y, eax
	}

	x_ = x;
	y_ = y;
}

static void speck_round_dec(uint32_t& x_, uint32_t& y_, uint32_t& k_)
{
	uint8_t a = 8;
	uint8_t b = 3;

	uint32_t x = x_;
	uint32_t y = y_;
	uint32_t k = k_;

	__asm {

			MOV eax, x   // y = x XOR y
			XOR eax, y
			MOV y, eax
	
			MOV cl, b
			ROR y, cl  // x

			MOV eax, x
			XOR eax, k // x = x XOR k

			SUB eax, y // x - y
			MOV x, eax  

			MOV cl, a
			ROL x, cl //
	}
	x_ = x;
	y_ = y;
}

void speck_setup_asm(const uint32_t key[4]
	, uint32_t key_schedule[ROUNDS]
	)
{
	uint32_t b = key[0];
	uint32_t a[3];
	uint32_t i;
//	char format[] = "a[i % 3] = 0x%08x b = 0x%08x  i = 0x%08x \n";
	a[0] = key[1];
	a[1] = key[2];
	a[2] = key[3];
	key_schedule[0] = b;
	__asm{
		mov ebx, ROUNDS-1;
	l1:
		mov eax, ROUNDS-1;
		sub eax, ebx;
		lea ecx, i
		mov [ecx], eax
		lea ecx, i;
		push ecx;

		lea eax, b
		push eax

		mov eax, ROUNDS-1;
		sub eax, ebx;
		xor edx, edx
		mov ecx, 3
		div ecx; теперь в edx остаток
		mov eax, edx
		imul eax, eax, 4;
		
		lea edx, a
		add eax, edx;
		push eax;

		call dword ptr speck_round_asm;
		add esp, 12;

		mov eax, ROUNDS;
		sub eax, ebx; i + 1
		imul eax, eax, 4; (i + 1) * 4
		mov ecx, dword ptr[key_schedule];
		add eax, ecx
		mov ecx, b;
		mov[eax], ecx

		;	 lea eax, format;
		; push eax;
		; call dword ptr printf;
		; add esp, 16;

		sub ebx, 1;
		jnz l1;

	}
}

void speck_setup(const uint32_t key[4]
	, uint32_t key_schedule[ROUNDS]
	)
{
	uint32_t b = key[0];
	uint32_t a[3];
	a[0] = key[1];
	a[1] = key[2];
	a[2] = key[3];
	key_schedule[0] = b;

	for (unsigned i = 0; i < ROUNDS - 1; i++) {

		speck_round_asm(a[i % 3], b, i);
		key_schedule[i + 1] = b;
	}

}

void speck_encrypt_asm(const uint32_t plaintext[2]
	, uint32_t key_schedule[ROUNDS]
	, uint32_t ciphertext[2]
	)
{
	ciphertext[0] = plaintext[0];
	ciphertext[1] = plaintext[1];

	uint32_t a = ciphertext[0];
	uint32_t b = ciphertext[1];

	char format[] = "ebx = 0x%08x 0x%08x 0x%08x \n";
	__asm{
		mov ebx, ROUNDS;
	l1:
		mov eax, ROUNDS;
		sub eax, ebx;
		mov edx, dword ptr[key_schedule];
		imul eax, eax, 4;
		add eax, edx;
		push eax;

		mov edx, dword ptr[ciphertext];
		push edx;

		mov edx, dword ptr[ciphertext];
		add edx, 4;
		push edx;

		call dword ptr speck_round_asm;
		add esp, 12;

		;	push ebx;
		; lea eax, format;
		; push eax;
		; call dword ptr printf;
		; add esp, 16;

		sub ebx, 1;
		jnz l1;

	}
}


void speck_encrypt(const uint32_t plaintext[2]
	, uint32_t key_schedule[ROUNDS]
	, uint32_t ciphertext[2]
	)
{
	ciphertext[0] = plaintext[0];
	ciphertext[1] = plaintext[1];

	uint32_t a = ciphertext[0];
	uint32_t b = ciphertext[1];
	for (unsigned i = 0; i < ROUNDS; i++) {
		speck_round_asm(ciphertext[1], ciphertext[0], key_schedule[i]);
	}
}

void speck_decrypt(const uint32_t ciphertext[2]
	, uint32_t key_schedule[ROUNDS]
	, uint32_t decrypted[2]
	)
{
	decrypted[0] = ciphertext[0];
	decrypted[1] = ciphertext[1];
	for (unsigned i = ROUNDS; i > 0; i--) {
		speck_round_dec_asm(decrypted[1], decrypted[0], key_schedule[i - 1]);
	}
}

void speck_decrypt_asm(const uint32_t ciphertext[2]
	, const uint32_t key_schedule[ROUNDS]
	, uint32_t decrypted[2]
	)
{
	decrypted[0] = ciphertext[0];
	decrypted[1] = ciphertext[1];

	char format[] = "dec[1] = 0x%08x dec[0] = 0x%08x  key[i - 1] = 0x%08x \n";
	__asm{
		mov ebx, ROUNDS;
	l1:
		mov eax, ebx;
		sub eax, 1;
		mov edx, dword ptr[key_schedule];
		imul eax, eax, 4;
		add eax, edx;
		push eax;

		mov edx, dword ptr[decrypted];
		push edx;

		mov edx, dword ptr[decrypted];
		add edx, 4;
		push edx;

		call dword ptr speck_round_dec_asm;
		add esp, 12;

		;	push ebx;
		; lea eax, format;
		; push eax;
		; call dword ptr printf;
		;  add esp, 16;
		
		;--------------------------------------
		sub ebx, 1;
		jnz l1;

	} 
}


void test()
{
	uint32_t plaintext[2] = { 0x7475432dUL, 0x3b726574UL };
	uint32_t key[4] = { 0x03020100UL, 0x0b0a0908UL, 0x13121110UL, 0x1b1a1918UL };
	uint32_t ciphertext[2];
	uint32_t decrypted[2];
	uint32_t key_schedule[ROUNDS];

	speck_setup_asm(key, key_schedule);
	speck_encrypt_asm(plaintext, key_schedule, ciphertext);
	speck_decrypt_asm(ciphertext, key_schedule, decrypted);

	printf("Plaintext:  0x%08x 0x%08x\n", plaintext[0], plaintext[1]);
	printf("Key:        0x%08x 0x%08x 0x%08x 0x%08x\n", key[0], key[1], key[2], key[3]);
	printf("Ciphertext: 0x%08x 0x%08x\n", ciphertext[0], ciphertext[1]);
	printf("Expected:   0x454e028b 0x8c6fa548\n");
	printf("Decrypted:  0x%08x 0x%08x\n", decrypted[0], decrypted[1]);
}


int main(void)
{
	test();
	return 0;
}
