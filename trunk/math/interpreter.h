#ifndef INTERPRETER_H
#define INTERPRETER_H


// Instruction Format
// instruction [31...26] destination [25..18] source2/i [17..9] source [8..0]

// Instructions are:
// binary: + - * / ^ {if it is a binary function, source2 is also a source}
// unary: sqr, lambda, sqrt, sin cos tan atan exp log conj 1/ *z *c +z +c, rect, polar, Re, Im, abs, arc
// unary with number i encoded: ^int
// const with number i encoded: z[n-i] cp[i] {i > 0}
// const: num, z, pixel, c {in julia sets constant, in mandelbrot = pixel}, pixel, pix_x, pix_y

// Sources are:
// bit 8 = 1: Numbers {[7..4] + i * [3..0]}
// else Registers: 0 = no register, use nr/ni

// Instruction: [5..0]:
// if < 8 [bits 5..3 unset]: src2 = source


// Binary ( < 8 )
#define OP_ADD 0x00
#define OP_SUB 0x01
#define OP_MUL 0x02
#define OP_DIV 0x03
#define OP_POW 0x04

// Unary
#define OP_SQR 0x08
#define OP_POW_INT 0x9 // using integer of src2
#define OP_LAMBDA 0x0a
#define OP_SQRT 0x0b
#define OP_NEG 0x0c
#define OP_INV 0x0d

#define OP_EXP 0x10
#define OP_LOG 0x11
#define OP_SIN 0x12
#define OP_COS 0x13
#define OP_SINH 0x14
#define OP_COSH 0x15

#define OP_CONJ 0x20
#define OP_RECT 0x21
#define OP_POLAR 0x22

#define OP_ADD_C 0x30
#define OP_ADD_Z 0x31
#define OP_MUL_C 0x32
#define OP_MUL_Z 0x33

#define OP_RE 0x40
#define OP_IM 0x41
#define OP_RAD 0x42
#define OP_ARC 0x43

#define OP_ID 0x50 // Identity

// Constants
#define OP_Z 0x80
#define OP_C 0x81
#define OP_Z_N_MINUS_I 0x90


// Shorts for often used functions
#define MULT(ar, br, ai, bi) { T t = ar * (br) - ai * (bi); ai = ar * (bi) + ai * (br); ar = t; }
#define ABSSQR(ar, ai) ( ar * ar + ai * ai )

#define SOURCE(src, regs, tr, ti) { if(src <= 0xff) { tr = regs[src - 1]; ti = regs[src]; } else {} }

#define INTERPRET(count, instructions, dr, di, cr, ci, zr, zi, n, xs, ys, regs) \
for(int j = 0; j < count; j++) {						\
	unsigned int op = instructions[j];					\
										\
	if(op & 0x1ff) {							\
		source<T>(op & 0x1ff, regs, dr, di);				\
	}									\
										\
	if(op < (8 << 26)) {							\
		T sr, si;							\
		source<T>((op >> 9) & 0x1ff, regs, sr, si);			\
										\
		switch(op >> 26) {						\
		case OP_ADD: dr += sr; di += si; break;				\
		case OP_SUB: dr -= sr; di -= si; break;				\
		case OP_MUL: MULT(dr, sr, di, si) break;			\
		case OP_DIV: {							\
				T abs = ABSSQR(sr, si);				\
				MULT(dr, sr, di, -si);				\
				dr /= abs; di /= abs;				\
			}							\
			break;							\
		case OP_POW:							\
			break;							\
		}								\
	} else {								\
		switch(op >> 26) {						\
		case OP_SQR: MULT(dr, dr, di, di) break;			\
		case OP_POW_INT: break;						\
		case OP_LAMBDA: MULT(dr, (1 - dr), di, -di) break;		\
		case OP_SQRT: break;						\
		case OP_NEG: dr = -dr; di = -di; break;				\
		case OP_INV: {							\
				T abs = ABSSQR(dr, di);				\
				dr /= abs; di = -di / abs;			\
			}							\
			break;							\
										\
		case OP_EXP: break;						\
		case OP_LOG: break;						\
		case OP_SIN: break;						\
		case OP_COS: break;						\
		case OP_SINH: break;						\
		case OP_COSH: break;						\
										\
		case OP_CONJ: di = -di; break;					\
		case OP_RECT: break;						\
		case OP_POLAR: break;						\
										\
		case OP_ADD_C: dr += cr; di += ci; break;			\
		case OP_ADD_Z: dr += zr; di += zi; break;			\
		case OP_MUL_C: MULT(dr, cr, di, ci) break;			\
		case OP_MUL_Z: MULT(dr, zr, di, zi) break;			\
										\
		case OP_RE: di = 0; break;					\
		case OP_IM: dr = di; di = 0; break;				\
		case OP_RAD: dr = sqrt(dr * dr + di * di); di = 0; break;	\
		case OP_ARC: break;						\
										\
		case OP_ID: /* Do nothing */ break;				\
										\
		case OP_Z: dr = zr; di = zi; break;				\
		case OP_C: dr = cr; di = ci; break;				\
		case OP_Z_N_MINUS_I: break;					\
		}								\
										\
		if(op & (0xff << 18)) {						\
			unsigned int dest = (op << 18) & 0xff;			\
										\
			regs[dest - 1] = dr;					\
			regs[dest] = di;					\
		}								\
	}									\
}

template<class T>
inline void source(unsigned int src, T* regs, T& tr, T& ti) {
	if(src > 0xff) {
		// It's a number
		int re = (src & 0xf0) >> 4;
		int im = src & 0xf;

		// TODO SGN
		tr = re;
		ti = im;
	} else {
		tr = regs[src - 1];
		ti = regs[src];
	}
}

template<class T>
inline void interpret(
		int count, unsigned int* instructions,
		T& dr, T& di,
		T cr, T ci,
		T zr, T zi,
		int n, T* xs, T* ys,
		T* regs) {
	for(int j = 0; j < count; j++) {
		unsigned int op = instructions[j];


		if(op & 0x1ff) { // if it is 0 we keep dr/di
			source<T>(op & 0x1ff, regs, dr, di);
		}

		if(op < (8 << 26)) {
			T sr, si;
			source<T>((op >> 9) & 0x1ff, regs, sr, si);

			switch(op >> 26) {
			case OP_ADD: dr += sr; di += si; break;
			case OP_SUB: dr -= sr; di -= si; break;
			case OP_MUL: MULT(dr, sr, di, si) break;
			case OP_DIV: {
					T abs = ABSSQR(sr, si);
					MULT(dr, sr, di, -si);
					dr /= abs; di /= abs;
				}
				break;
			case OP_POW:
				break;
			}
		} else {
			switch(op >> 26) {
				// Unary
			case OP_SQR: MULT(dr, dr, di, di) break;
			case OP_POW_INT: break;
			case OP_LAMBDA: MULT(dr, (1 - dr), di, -di) break;
			case OP_SQRT: break;
			case OP_NEG: dr = -dr; di = -di; break;
			case OP_INV: {
					T abs = ABSSQR(dr, di);
					dr /= abs; di = -di / abs;
				}
				break;

			case OP_EXP: break;
			case OP_LOG: break;
			case OP_SIN: break;
			case OP_COS: break;
			case OP_SINH: break;
			case OP_COSH: break;

			case OP_CONJ: di = -di; break;
			case OP_RECT: break;
			case OP_POLAR: break;

			case OP_ADD_C: dr += cr; di += ci; break;
			case OP_ADD_Z: dr += zr; di += zi; break;
			case OP_MUL_C: MULT(dr, cr, di, ci) break;
			case OP_MUL_Z: MULT(dr, zr, di, zi) break;

			case OP_RE: di = 0; break;
			case OP_IM: dr = di; di = 0; break;
			case OP_RAD: dr = sqrt(dr * dr + di * di); di = 0; break;
			case OP_ARC: break;

			case OP_ID: break;

				// Constants
			case OP_Z: dr = zr; di = zi; break;
			case OP_C: dr = cr; di = ci; break;
			case OP_Z_N_MINUS_I: break;
			}

			if(op & (0xff << 18)) {
				unsigned int dest = (op << 18) & 0xff;

				regs[dest - 1] = dr;
				regs[dest] = di;
			}
		}
	}
}


#endif // INTERPRETER_H
