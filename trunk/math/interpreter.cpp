#include "interpreter.h"

template<class T>
Interpreter<T>::Interpreter() {
	rrrOpTable_[QString("add")] = OP_ADD;
	rrrOpTable_[QString("sub")] = OP_SUB;
	rrrOpTable_[QString("mul")] = OP_MUL;
	rrrOpTable_[QString("div")] = OP_DIV;
	rrrOpTable_[QString("pow")] = OP_POW;

	rrOpTable_[QString("sqr")] =  OP_SQR;
	rirOpTable_[QString("pow_i")] =  OP_POW_INT;

	rrOpTable_[QString("lambda")] =  OP_LAMBDA;
	rrOpTable_[QString("sqrt")] =  OP_SQRT;
	rrOpTable_[QString("neg")] =  OP_NEG;
	rrOpTable_[QString("inv")] =  OP_INV;

	rrOpTable_[QString("exp")] =  OP_EXP;
	rrOpTable_[QString("log")] =  OP_LOG;
	rrOpTable_[QString("sin")] =  OP_SIN;
	rrOpTable_[QString("cos")] =  OP_COS;
	rrOpTable_[QString("sinh")] =  OP_SINH;
	rrOpTable_[QString("cosh")] =  OP_COSH;

	rrOpTable_[QString("conj")] =  OP_CONJ;
	rrOpTable_[QString("rect")] =  OP_RECT;
	rrOpTable_[QString("polar")] =  OP_POLAR;
	rrOpTable_[QString("abs")] =  OP_ABS;
	rrOpTable_[QString("floor")] =  OP_FLOOR;

	rrOpTable_[QString("add_c")] =  OP_ADD_C;
	rrOpTable_[QString("add_z")] =  OP_ADD_Z;
	rrOpTable_[QString("mul_c")] =  OP_MUL_C;
	rrOpTable_[QString("mul_z")] =  OP_MUL_Z;

	rrOpTable_[QString("Re")] =  OP_RE;
	rrOpTable_[QString("Im")] =  OP_IM;
	rrOpTable_[QString("Rad")] =  OP_RAD;
	rrOpTable_[QString("Arc")] =  OP_ARC;

	rrOpTable_[QString("id")] =  OP_ID;

	// Constants
	rOpTable_[QString("z")] =  OP_Z;
	rOpTable_[QString("c")] =  OP_C;

	riOpTable_[QString("z_i")] =  OP_Z_I;
}

template<class T>
int Interpreter<T>::addReg(T re, T im) {
	regs_.push_back(re);
	regs_.push_back(im);

	return (regs_.size() + 1) / 2;
}

template<class T>
bool Interpreter<T>::addRRROp(QString op, int dst, int src1, int src2) {
	QMap<QString, unsigned int>::const_iterator i = rrrOpTable_.find(op);

	if(i != rrrOpTable_.end()) {
		int opCode = i.value() << 24;

		if(src1 >= 0) {
			// * 2, shl 8
			opCode |= src1 << 8;
		}

		if(src2 >= 0) {
			// shl 16
			opCode |= src2 << 16;
		}

		if(dst >= 0) {
			opCode |= dst;
		}

		ops_.push_back(opCode);

		return true;
	} else {
		return false;
	}
}

template<class T>
bool Interpreter<T>::addRIROp(QString op, int dst, int src, int num) {
	QMap<QString, unsigned int>::const_iterator i = rirOpTable_.find(op);

	if(i != rrrOpTable_.end()) {
		int opCode = i.value() << 24;

		if(src >= 0) {
			// * 2, shl 8
			opCode |= src << 8;
		}

		opCode |= (num & 0xff) << 16;

		if(dst >= 0) {
			opCode |= dst;
		}

		ops_.push_back(opCode);

		return true;
	} else {
		return false;
	}
}

template<class T>
bool Interpreter<T>::addRROp(QString op, int dst, int src) {
	QMap<QString, unsigned int>::const_iterator i = rrOpTable_.find(op);

	if(i != rrrOpTable_.end()) {
		int opCode = i.value() << 24;

		if(src >= 0) {
			// * 2, shl 8
			opCode |= src << 8;
		}

		if(dst >= 0) {
			opCode |= dst;
		}

		ops_.push_back(opCode);

		return true;
	} else {
		return false;
	}
}

template<class T>
bool Interpreter<T>::addRIOp(QString op, int dst, int num) {
	QMap<QString, unsigned int>::const_iterator i = riOpTable_.find(op);

	if(i != rrrOpTable_.end()) {
		int opCode = i.value() << 24;

		opCode |= (num & 0xff) << 8;

		if(dst >= 0) {
			opCode |= dst;
		}

		ops_.push_back(opCode);

		return true;
	} else {
		return false;
	}
}

template<class T>
bool Interpreter<T>::addROp(QString op, int dst) {
	QMap<QString, unsigned int>::const_iterator i = rOpTable_.find(op);

	if(i != rrrOpTable_.end()) {
		int opCode = i.value() << 24;

		if(dst >= 0) {
			opCode |= dst;
		}

		ops_.push_back(opCode);

		return true;
	} else {
		return false;
	}
}


template<class T>
int Interpreter<T>::opCount() const {
	return ops_.size();
}

template<class T>
void Interpreter<T>::initOps(unsigned int* ops) const {
	for(int i = 0; i < opCount(); i++) {
		ops[i] = ops_[i];
	}
}

template<class T>
int Interpreter<T>::regCount() const {
	return regs_.size();
}

template<class T>
void Interpreter<T>::initRegs(T* regs) const {
	for(int i = 0; i < regCount(); i++) {
		regs[i] = regs_[i];
	}
}

template<class T>
void Interpreter<T>::interpret(T& dr, T& di, T cr, T ci, T zr, T zi, int n, T* xs, T* ys) const {
	for(int i = 0; i < ops_.size(); i++) {
		unsigned int op = ops_[i];

		// TODO: Work around because regs_[i] = bla does not work. Why's that?
		T* regs = new T[regs_.size()];

		for(int i = 0; i < regs_.size(); i++) {
			regs[i] = regs_[i];
		}

		INTERPRET(T, op, dr, di, cr, ci, zr, zi, n, xs, ys, regs)

		delete regs;
	}
}

/*template<class T>
void Interpreter<T>::skipWhite() {
	while(expr_[offset_] == ' ') offset_++;
}

template<class T>
bool Interpreter<T>::nextInt() {
	int length = 0;

	while('0' <= expr_[offset_ + length] && expr_[offset_ + length] <=  '9') {
		length ++;
	}

	lexem_ = expr_.mid(offset_, length);

	return length > 0;
}

template<class T>
bool Interpreter<T>::nextReal() {
	int mark = offset_;

	if(nextInt()) {
		offset_ += lexem_.length();

		if(nextChar('.')) {
			offset_ ++;

			nextInt();

			offset_ += lexem_.length();
		}

		if(nextChar('e') || nextChar('E')) {
			int mark2 = offset_;

			offset_ ++;

			if(nextChar('-')) {
				offset_ ++;
			}

			if(nextInt()) {
				offset_ += lexem_.size();
			} else {
				offset_ = mark2;
			}
		}

		lexem_ = expr_.mid(mark, offset_ - mark);
		offset_ = mark;

		return true;
	} else {
		return false;
	}
}

template<class T>
bool Interpreter<T>::nextChar(char ch) {
	if(expr_[offset_] == ch) {
		lexem_ = QString(ch);
		return true;
	} else {
		return false;
	}
}

template<class T>
bool Interpreter<T>::nextString() {
	int length = 0;

	if(expr_[offset_].isLetter()) {
		length++;

		while(expr_[offset_ + length].isLetterOrNumber()) {
			length++;
		}

		lexem_ = expr_.mid(offset_, length);

		return true;
	} else {
		return false;
	}
}

template<class T>
TreeNode* Interpreter<T>::sum() {
	//sum := product ( [ + - ] product )*
	TreeNode* l = product();

	skipWhite();

	while(nextChar('+') || nextChar('-')) {

		char op = expr_[offset_];

		offset_ ++;

		TreeNode* r = product();

		l = new SumNode(l, r, op == '+');

		skipWhite();
	}

	// a + b + c => (a + b) + c
	return l;
}

template<class T>
TreeNode* Interpreter<T>::product() {
	//sum := product ( [ + - ] product )*
	TreeNode* l = exp();

	skipWhite();

	while(!nextChar('+') && !nextChar('-') && !nextChar(')')) {

		char op = '*';

		if(nextChar('*')) {
			offset_ ++;
		} else if(nextChar('/')) {
			offset_ ++;
			op = '/';
		}

		TreeNode* r = product();

		l = new ProductNode(l, r, op == '*');

		skipWhite();
	}

	// a + b + c => (a + b) + c
	return l;
}

template<class T>
TreeNode* Interpreter<T>::exp() {
	// exp := app ( ^ app )*

	TreeNode* l = app();

	skipWhite();

	while(nextChar('^')) {
		offset_ ++;

		TreeNode* r = app();

		l = new PowNode(l, r);
	}

	// a ^ b ^ c => (a ^ b) ^ c = a ^ (b * c)

	return l;
}

template<class T>
TreeNode* Interpreter<T>::app() {
	//   app := FN app // Possible clash with product rule; includes expressions like z(0) or z(n-1)
	//	| term

	QString fn;

	skipWhite();

	if(nextString()) {
		if(lexem_ == "-") {
			offset_ += lexem_.size();
			return new AppNode(lexem_, app());
		} else if(lexem_ == "sin") {
			offset_ += lexem_.size();
			return new AppNode(lexem_, app());
		} else if(lexem_ == "cos") {
			offset_ += lexem_.size();
			return new AppNode(lexem_, app());
		} else if(lexem_ == "sinh") {
			offset_ += lexem_.size();
			return new AppNode(lexem_, app());
		} else if(lexem_ == "cosh") {
			offset_ += lexem_.size();
			return new AppNode(lexem_, app());
		} else if(lexem_ == "exp") {
			offset_ += lexem_.size();
			return new AppNode(lexem_, app());
		} else if(lexem_ == "log") {
			offset_ += lexem_.size();
			return new AppNode(lexem_, app());
		} else if(lexem_ == "conj") {
			offset_ += lexem_.size();
			return new AppNode(lexem_, app());
		} else {
			return term();
		}
	} else {
		return term();
	}
}

template<class T>
TreeNode* Interpreter<T>::term() {

	if(nextChar('(')) {
		offset_ ++;

		TreeNode* n = sum;

		skipWhite();

		if(nextChar(')')) {
			offset_ ++;
		} else {
			// TODO set error
		}

		skipWhite();

		while(nextChar('\'')) {
			// n = n->diff();
			offset_++;
			skipWhite();
		}

		return n;
	} else if(nextChar('c')) {
		offset_ ++;

	} else if(nextChar('z')) {
		offset_ ++;

		skipWhite();

		if(nextChar('[')) {
			offset_ ++;

			TreeNode* idx = index();
		}
	} else {
		T re = num();

		skipWhite();

		if(nextChar(',')) {
			offset_ ++;

			T im = num();
		}
	}
}

template<class T>
T Interpreter<T>::num() {
	// num := SIGNED_INT ( . INT ) ?  ( [eE] #peek# SIGNED_INT ) ?
	//     | pi
	//     | e // Clash with product and num

	if(nextReal()) {

	} else if(nextString()) {

	} else {
		// Error
	}
}

template<class T>
TreeNode* Interpreter<T>::index() {

}*/

void Parser::skipWhite(QString expr, int &pos) {
	while(expr[pos].isSpace()) pos++;
}

Node* Parser::sum(QString expr, int& pos) {
	if(expr[pos] == '-') {

	}
}

Node* Parser::product(QString expr, int& pos) {}
Node* Parser::app(QString expr, int& pos) {}
Node* Parser::power(QString expr, int& pos) {}
Node* Parser::term(QString expr, int& pos) {}
Node* Parser::var(QString expr, int& pos) {}
Node* Parser::num(QString expr, int& pos) {}


template class Interpreter<double>;
template class Interpreter<long double>;
