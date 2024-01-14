## Stacktrace of `fwd_butterfly_loop`
Using gdb I printed the stacktrace of the `fwd_butterfly_loop` method at `PGFFT.cpp:873`.

### Called by `GenSecKey` at `main.cpp:23`
``` terminal
gdb ./build/myprogram
(gdb) break fwd_butterfly_loop
(gdb) run
(gdb) backtrace
    #0  helib::fwd_butterfly_loop(long, helib::cmplx_t * __restrict__, helib::cmplx_t * __restrict__, const helib::cmplx_t * __restrict__) (size=2048, xp0=0x555555994390, xp1=0x55555599c390, wtab=0x55555586c0a0) at ../HElib/src/PGFFT.cpp:873
    #1  0x00005555556caaeb in helib::new_fft_short (xp=<optimized out>, yn=<optimized out>, xn=2048, lgN=<optimized out>, tab=std::vector of length 13, capacity 13 = {...}) at ../HElib/src/PGFFT.cpp:1125
    #2  0x00005555556cadf0 in helib::new_fft (tab=std::vector of length 13, capacity 13 = {...}, lgN=12, xp=0x555555994390) at ../HElib/src/PGFFT.cpp:1140
    #3  helib::pow2_comp (tab=std::vector of length 13, capacity 13 = {...}, rev1=std::vector of length 32, capacity 32 = {...}, rev=std::vector of length 4, capacity 4 = {...}, k=12, n=<optimized out>, dst=0x555555984380, src=0x555555984380) at ../HElib/src/PGFFT.cpp:1510
    #4  helib::PGFFT::apply (this=0x5555557a41a0, src=0x555555984380, dst=0x555555984380) at ../HElib/src/PGFFT.cpp:1769
    #5  0x00005555556900ac in helib::PGFFT::apply (v=0x555555984380, this=0x5555557a41a0) at ../HElib/include/helib/PGFFT.h:44
    #6  helib::quarter_embeddingLargestCoeff (palg=..., f=std::vector of length 8192, capacity 8192 = {...}) at ../HElib/src/norms.cpp:225
    #7  helib::embeddingLargestCoeff (f=std::vector of length 8192, capacity 8192 = {...}, palg=...) at ../HElib/src/norms.cpp:255
    #8  0x0000555555690744 in helib::embeddingLargestCoeff (f=..., palg=...) at ../HElib/src/norms.cpp:440
    #9  0x00005555557094eb in helib::sampleSmallBounded (poly=..., context=...) at ../HElib/src/sample.cpp:358
    #10 0x00005555555f6595 in helib::DoubleCRT::sampleSmallBounded (this=this@entry=0x7fffffffd2a0) at ../HElib/src/DoubleCRT.cpp:1394
    #11 0x000055555565c358 in helib::SecKey::GenSecKey (this=this@entry=0x7fffffffd730, ptxtSpace=ptxtSpace@entry=0, maxDegKswitch=maxDegKswitch@entry=3) at ../HElib/src/keys.cpp:1152
    #12 0x0000555555594510 in main (argc=<optimized out>, argv=<optimized out>) at src/main.cpp:23
```

### Called by `*=` at `main.cpp:51`
``` terminal
gdb ./build/myprogram
(gdb) break main.cpp:45
(gdb) run
(gdb) break fwd_butterfly_loop
(gdb) continue
(gdb) backtrace
    #0  helib::fwd_butterfly_loop(long, helib::cmplx_t * __restrict__, helib::cmplx_t * __restrict__, const helib::cmplx_t * __restrict__) (size=4096, xp0=0x5555560e08b0, xp1=0x5555560f08b0, wtab=0x555555836060) at ../HElib/src/PGFFT.cpp:873
    #1  0x00005555556caaeb in helib::new_fft_short (xp=<optimized out>, yn=<optimized out>, xn=4096, lgN=<optimized out>, tab=std::vector of length 14, capacity 14 = {...}) at ../HElib/src/PGFFT.cpp:1125
    #2  0x00005555556cadf0 in helib::new_fft (tab=std::vector of length 14, capacity 14 = {...}, lgN=13, xp=0x5555560e08b0) at ../HElib/src/PGFFT.cpp:1140
    #3  helib::pow2_comp (tab=std::vector of length 14, capacity 14 = {...}, rev1=std::vector of length 32, capacity 32 = {...}, rev=std::vector of length 8, capacity 8 = {...}, k=13, n=<optimized out>, dst=0x555555d44ec0, src=0x555555d44ec0) at ../HElib/src/PGFFT.cpp:1510
    #4  helib::PGFFT::apply (this=0x5555557a2200, src=0x555555d44ec0, dst=0x555555d44ec0) at ../HElib/src/PGFFT.cpp:1769
    #5  0x0000555555690d68 in helib::PGFFT::apply (v=0x555555d44ec0, this=0x5555557a2200) at ../HElib/include/helib/PGFFT.h:44
    #6  helib::half_embeddingLargestCoeff_x2 (palg=..., f2=std::vector of length 8192, capacity 8192 = {...}, f1=std::vector of length 8192, capacity 8192 = {...}, norm2=@0x555555d7d538: 0, norm1=@0x555555d7d530: 0) at ../HElib/src/norms.cpp:371
    #7  helib::embeddingLargestCoeff_x2 (norm1=@0x555555d7d530: 0, norm2=@0x555555d7d538: 0, f1=std::vector of length 8192, capacity 8192 = {...}, f2=std::vector of length 8192, capacity 8192 = {...}, palg=...) at ../HElib/src/norms.cpp:431
    #8  0x00005555555da39b in helib::Ctxt::modDownToSet (this=0x7fffffffd680, s=...) at ../HElib/src/Ctxt.cpp:496
    #9  0x00005555555e353b in helib::Ctxt::multLowLvl (this=0x7fffffffd680, other_orig=..., destructive=<optimized out>) at ../HElib/src/Ctxt.cpp:1748
    #10 0x00005555555e415d in helib::Ctxt::multiplyBy (this=this@entry=0x7fffffffd680, other=...) at ../HElib/src/Ctxt.cpp:1775
    #11 0x000055555559473b in helib::Ctxt::operator*= (other=..., this=0x7fffffffd680) at ../HElib/include/helib/Ctxt.h:576
    #12 main (argc=<optimized out>, argv=<optimized out>) at src/main.cpp:51
```
