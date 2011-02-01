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
TreeNode<T>* Interpreter<T>::sum() {
	//sum := product ( [ + - ] product )*
	TreeNode<T>* l = product();

	skipWhite();

	while(nextChar('+') || nextChar('-')) {

		char op = expr_[offset_];

		offset_ ++;

		TreeNode<T>* r = product();

		l = new SumNode(l, r, op == '+');

		skipWhite();
	}

	// a + b + c => (a + b) + c
	return l;
}

template<class T>
TreeNode<T>* Interpreter<T>::product() {
	//sum := product ( [ + - ] product )*
	TreeNode<T>* l = exp();

	skipWhite();

	while(!nextChar('+') && !nextChar('-') && !nextChar(')')) {

		char op = '*';

		if(nextChar('*')) {
			offset_ ++;
		} else if(nextChar('/')) {
			offset_ ++;
			op = '/';
		}

		TreeNode<T>* r = product();

		l = new ProductNode(l, r, op == '*');

		skipWhite();
	}

	// a + b + c => (a + b) + c
	return l;
}

template<class T>
TreeNode<T>* Interpreter<T>::exp() {
	// exp := app ( ^ app )*

	TreeNode<T>* l = app();

	skipWhite();

	while(nextChar('^')) {
		offset_ ++;

		TreeNode<T>* r = app();

		l = new PowNode(l, r);
	}

	// a ^ b ^ c => (a ^ b) ^ c = a ^ (b * c)

	return l;
}

template<class T>
TreeNode<T>* Interpreter<T>::app() {
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
TreeNode<T>* Interpreter<T>::term() {

	if(nextChar('(')) {
		offset_ ++;

		TreeNode<T>* n = sum;

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

			TreeNode<T>* idx = index();
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
TreeNode<T>* Interpreter<T>::index() {

}*/

template<class T>
Tree<T>::Tree(Node<T>* n) : root_(n) {}


template<class T>
Tree<T>* Tree<T>::parse(const QString& expr) {
	int pos = 0;
	Node<T>* root = sum(expr, pos);

	if(pos < expr.length()) {
		qDebug("WARNING: Parsing did not reach end of string");
	}

	qDebug() << QString(root->toString());

	return new Tree<T>(root);
}


template<class T>
bool Tree<T>::skipWhite(const QString& expr, int &pos) {
	while(pos < expr.length() && expr[pos].isSpace()) pos++;
	return pos < expr.length();
}

template<class T>
Node<T>* Tree<T>::sum(const QString& expr, int& pos) {
	if(!skipWhite(expr, pos)) return 0;

	bool neg = false;

	if(expr[pos] == '-') {
		neg = true;
		pos++;
	}

	Node<T>* l = product(expr, pos);

	if(l != 0 && neg) {
		l = new NegNode<T>(l);
	}

	if(!skipWhite(expr, pos)) return l;

	while(expr[pos] == '-' || expr[pos] == '+') {
		neg = expr[pos] == '-';
		pos ++;

		Node<T>* r = product(expr, pos);

		if(r == 0) {
			qDebug("WARNING: +- found but no argument");
			return l;
		}

		if(!neg) {
			l = new AddNode<T>(l, r);
		} else {
			l = new SubNode<T>(l, r);
		}

		if(!skipWhite(expr, pos)) return l;
	}

	return l;
}

template<class T>
Node<T>* Tree<T>::product(const QString& expr, int& pos) {
	if(!skipWhite(expr, pos)) return 0;

	Node<T>* l = app(expr, pos);

	if(!skipWhite(expr, pos)) return l;

	while(true) {
		bool mul = expr[pos] == '*';
		bool div = expr[pos] == '/';

		if(mul || div) {
			pos++;
		}

		Node<T>* r = app(expr, pos);

		if(r == 0) {
			if(mul || div) {
				qDebug("WARNING: */ found but no argument");
			}

			return l;
		}

		if(!div) {
			l = new MulNode<T>(l, r);
		} else {
			l = new DivNode<T>(l, r);
		}

		if(!skipWhite(expr, pos)) return l;
	}

	return l;
}

template<class T>
Node<T>* Tree<T>::app(const QString& expr, int& pos) {
	if(!skipWhite(expr, pos)) return 0;

	// TODO: maintain list of valid applications
	// BUG!!! Start at different positiion!
	if(expr.startsWith("sin")) {
		pos += 3;

		Node<T>* n = app(expr, pos);

		if(n == 0) {
			qDebug("WARNING: Application without argument");
			return 0;
		} else {
			return new SinNode<T>(n);
		}
	} else {
		Node<T>* n = power(expr, pos);
		return n;
	}
}

template<class T>
Node<T>* Tree<T>::power(const QString& expr, int& pos) {
	// Right-associative

	Node<T>* l = term(expr, pos);

	if(!skipWhite(expr, pos)) return l;

	if(expr[pos] == '^') {
		pos++;
		Node<T>* r = power(expr, pos);

		if(r != 0) {
			return new PowNode<T>(l, r);
		} else {
			qDebug("WARNING: ^ found but no argument.");
		}
	}

	return l;
}

template<class T>
Node<T>* Tree<T>::term(const QString& expr, int& pos) {
	if(!skipWhite(expr, pos)) return 0;

	if(expr[pos] == '(') {
		pos++;
		Node<T>* l = sum(expr, pos);

		if(!skipWhite(expr, pos)) {
			qDebug("WARNING: Missing )");
			return l;
		}

		if(expr[pos] == ')') {
			pos++;
		} else {
			qDebug("WARNING: Missing )");
		}

		return l;
	} else {
		Node<T>* l = num(expr, pos);

		if(l != 0) return l;

		l = var(expr, pos);
		return l;
	}
}

template<class T>
Node<T>* Tree<T>::var(const QString& expr, int& pos) {
	// TODO so far only one char
	// TODO Consider constants like e, pi and i
	if(expr[pos].isLetter()) {
		QString name = expr[pos];
		pos++;

		return new VarNode<T>(name);
	}

	return 0;
}

template<class T>
Node<T>* Tree<T>::num(const QString& expr, int& pos) {
	// TODO so far only one digit
	if(expr[pos].isDigit()) {
		int i = expr.mid(pos, 1).toInt();
		pos++;

		return new NumNode<T>(i);
	}

	return  0;
}

template class Interpreter<double>;
template class Interpreter<long double>;

template class Tree<int>;
