section .text
global streq_32b
streq_32b:
    vmovdqa ymm0, [rdi]
    vmovdqa ymm1, [rsi]

    vpcmpeqb ymm2, ymm0, ymm1
    vpmovmskb eax, ymm2

    not eax

    ret
