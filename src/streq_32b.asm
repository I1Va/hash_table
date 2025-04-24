section .text
global streq_32b
streq_32b:
    vmovdqa ymm0, [rdi]         ; ymm0 - вектор, содержащий первую строку из 32 байт
    vmovdqa ymm1, [rsi]         ; ymm0 - вектор, содержащий вторую строку из 32 байт

    vpcmpeqb ymm2, ymm0, ymm1   ; ymm2 - результат побайтового сравнения
    vpmovmskb eax, ymm2         ; eax - маска, состоящая из каждого 8-го старшего бита вектора ymm2

    not eax                     ; если строки совпадают, то после инструкции not eax примет значение 0
    ret
