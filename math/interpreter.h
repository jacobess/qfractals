#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <sys/types.h>
#include <math.h>
#include <QMap>
#include <QList>
#include <QString>

// Instruction Format
// instruction [31...26] destination [25..18] source2/i [17..9] source [8..0]

// Instructions are:
// binary: + - * / ^ {if it is a binary function, source2 is also a source}
// unary: sqr, lambda, sqrt, sin cos tan atan exp log conj 1/ *z *c +z +c, rect,
//        polar, Re, Im, abs, arc
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
#define OP_ABS 0x23
#define OP_FLOOR 0x24

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
#define OP_Z_I 0x90


// Shorts for often used functions
#define MULT(ar, br, ai, bi)							\
{										\
	T t = ar * (br) - ai * (bi);						\
	ai = ar * (bi) + ai * (br);						\
	ar = t;									\
}

#define RD_SRC(src, regs, tr, ti)						\
{										\
	int k = (src) * 2 - 1;								\
	if(k >= 0) {								\
		tr = regs[k - 1];						\
		ti = regs[k];					\
	}									\
}

#define WT_DST(dst, regs, tr, ti)						\
{										\
	int k = (dst) * 2 - 1;								\
	if(k >= 0) {								\
		regs[k - 1] = tr;						\
		regs[k] = ti;					\
	}									\
}


// TODO template T!
#define INTERPRET(T, op, dr, di, cr, ci, zr, zi, n, xs, ys, regs)			\
	switch(op >> 24) {							\
	case OP_ADD:								\
		{								\
			T sr = dr, si = di;					\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			RD_SRC(((op) >> 16) & 0xff, regs, sr, si)		\
			dr += sr; di += si;					\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_SUB:								\
		{								\
			T sr = dr, si = di;					\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			RD_SRC(((op) >> 16) & 0xff, regs, sr, si)		\
			dr -= sr; di -= si;					\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_MUL:								\
		{								\
			T sr = dr, si = di;					\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			RD_SRC(((op) >> 16) & 0xff, regs, sr, si)		\
			MULT(dr, sr, di, si)					\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_DIV:								\
		{								\
			T sr = dr, si = di;					\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			RD_SRC(((op) >> 16) & 0xff, regs, sr, si)		\
			MULT(dr, sr, di, -si);					\
			T abs = sr * sr + si * si;				\
			dr /= abs; di /= abs;					\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_POW:								\
		{								\
			T sr = dr, si = di;					\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			RD_SRC(((op) >> 16) & 0xff, regs, sr, si)		\
			MULT(dr, sr, di, -si)				\
			T abs = sr * sr + si * si;				\
			dr /= abs; di /= abs;					\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_SQR:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			MULT(dr, dr, di, di)				\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_POW_INT:							\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			T ar = dr;						\
			T ai = di;						\
										\
			dr = 1;							\
			di = 0;							\
										\
			int i = int8_t((op >> 16) & 0xff);			\
										\
			if(i < 0) {						\
				i = -i;						\
				T abs = ar * ar + ai * ai;			\
				ar = ar / abs;					\
				ai = -ai / abs;					\
			}							\
										\
			while(i) {						\
				if(i & 1) {					\
					MULT(dr, ar, di, ai)			\
				}						\
										\
				MULT(ar, ar, ai, ai)				\
				i >>= 1;					\
			}							\
										\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_LAMBDA:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			MULT(dr, (1 - dr), di, -di)			\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_SQRT:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			MULT(dr, dr, di, di)					\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_NEG:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			dr = -dr; di = -di;					\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_INV:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			T abs = dr * dr + di * di;				\
			dr = dr / abs; di = -di / abs;				\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_EXP:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			T t = exp(dr);						\
			dr = t * cos(di);					\
			di = t * sin(di);					\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_LOG: break;							\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_SIN:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			T t = sin(dr) * cosh(di);				\
			di = cos(dr) * sinh(di);				\
			dr = t;							\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_COS:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			T t = cos(dr) * cosh(di);				\
			di = -sin(dr) * sinh(di);				\
			dr = t;							\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_SINH:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			T t = sinh(dr) * cos(di);				\
			di = cosh(dr) * sin(di);				\
			dr = t;							\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_COSH:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			T t = cosh(dr) * cos(di);				\
			di = sinh(dr) * sin(di);				\
			dr = t;							\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_CONJ:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			di = -di;						\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_RECT:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			T t = dr;						\
			dr = dr * cos(di);					\
			di = t * sin(di);					\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_ABS:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			if(dr < 0) dr = -dr;					\
			if(di < 0) di = -di;					\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_POLAR:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			T t = atan2(di, dr);					\
			dr = sqrt(dr * dr + di * di);				\
			di = t;							\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_FLOOR:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			dr = floor(dr);						\
			di = floor(di);						\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_ADD_C:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			dr += cr; di += ci;					\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_ADD_Z:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			dr += zr; di += zi;					\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_MUL_C:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			MULT(dr, cr, di, ci)					\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_MUL_Z:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			MULT(dr, zr, di, zi)					\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_RE:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			di = 0;							\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_IM:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			dr = di; di = 0;					\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_RAD:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			dr = sqrt(dr * dr + di * di); di = 0;			\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_ARC:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			dr = atan2(di, dr); di = 0;				\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_ID:								\
		{								\
			RD_SRC(((op) >> 8) & 0xff, regs, dr, di)		\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_Z:								\
		{								\
			dr = zr; di = zi;					\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_C:								\
		{								\
			dr = cr; di = ci;					\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	case OP_Z_I:								\
		{								\
			int index = int8_t((op >> 8) & 0xff);			\
			if(index < 0) index += n;				\
			dr = xs[index];						\
			di = ys[index];						\
			WT_DST((op) & 0xff, regs, dr, di)			\
		} break;							\
	}

/*
  Grammar of the parser:
sum     := prod ([+-] prod)*

prod    := exp ( {peek: [^+-),]} [* /]? exp)*

exp     := unapp (^ unapp)*

unapp   := FN unapp
	| term

term    := ( sum ) (')*
	| "z" ( index )?
	| "c"
	| _[_ num, num _]_
	| num

num     := FLOAT
	| "e"
	| "pi"

index   := _(_ (n -)? INT _)_

FN      := ( - | sin | cos | sinh | cosh | exp | log )
INT     := [0-9][0-9]*
FLOAT   := INT ( . [0-9]*) ([eE] -? INT)

*/


template<class T>
class Interpreter {
	// first 3 chars: Src1 Src2 Dst
	// r = register, i = integer, n = none
	QMap<QString, unsigned int> rrrOpTable_;
	QMap<QString, unsigned int> rirOpTable_;
	QMap<QString, unsigned int> rrOpTable_;
	QMap<QString, unsigned int> rOpTable_;
	QMap<QString, unsigned int> riOpTable_;

	QList<unsigned int> ops_;
	QList<T> regs_;

public:
	Interpreter();

	// Returns the index of the register
	int addReg(T re = 0, T im = 0);

	// if a source is negative, no register is used
	bool addRRROp(QString op, int dst = -1, int src1 = -1, int src2 = -1);
	bool addRIROp(QString op, int dst = -1, int src = -1, int num = 0);
	bool addRROp(QString op, int dst = -1, int src = -1);
	bool addRIOp(QString op, int dst = -1, int num = 0);
	bool addROp(QString op, int dst = -1);

	int opCount() const;
	void initOps(unsigned int* ops) const;

	int regCount() const;
	void initRegs(T* regs) const;

	void interpret(T& nr, T& ni, T cr, T ci, T zr, T zi, int n, T* xs, T* ys) const;
};

#endif // INTERPRETER_H
