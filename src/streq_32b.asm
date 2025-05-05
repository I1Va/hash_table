global streq_32b

section .text

streq_32b:
    vmovdqa ymm0, [rdi]
    xor rax, rax
    vptest ymm0, [rsi]
    seta al

    vmovdqa ymm0, [rdi]
    xor rax, rax
    vptest ymm0, [rsi]
    seta al

    vmovdqa ymm0, [rdi]
    xor rax, rax
    vptest ymm0, [rsi]
    seta al

    vmovdqa ymm0, [rdi]
    xor rax, rax
    vptest ymm0, [rsi]
    seta al

    vmovdqa ymm0, [rdi]
    xor rax, rax
    vptest ymm0, [rsi]
    seta al

    ret
