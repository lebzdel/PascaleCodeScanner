#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <set>
#include <map>
#include <vector>

using namespace std;

class Parser {

public:
	void CheckProgram(string text_on_pascal, map <int, vector<int> > mp) {
		text = text_on_pascal;
		result = prog();
		if (!result) {

			if (fix_ind == 0) {
				for (auto& item : mp)
				{
					if (find(item.second.begin(), item.second.end(), mainIndex) != item.second.end())
					{
						cout << endl << "\n\nОшибка в строке: " << item.first;
						break;
					}
				}
			}
			else {
				if (err_r_w) { fix_ind = ind_br; }
				if (err_last) { fix_ind = ind_last; }
				for (auto& item : mp)
				{
					if (find(item.second.begin(), item.second.end(), fix_ind) != item.second.end())
					{
						cout << endl << "\n\nОшибка в строке: " << item.first;
						break;
					}
				}
			}

			cout << "\n\n\nТекст не соответствует программе на языке программирования Pascal\n\n";
		}
		else {
			cout << "\n\n\nТекст прошел тестирование.\n\n";
		}
	}

private:
	string text;
	const string _PROGRAM_ = "program";
	const string _TYPE_ = "type";
	const string _VAR_ = "var";
	const string _BEGIN_ = "begin";
	const string _END_ = "end";
	const string _INTEGER_ = "integer";
	const string _BOOLEAN_ = "boolean";
	const string _RECORD_ = "record";
	const string _IF_ = "if";
	const string _THEN_ = "then";
	const string _ELSE_ = "else";
	const string _READ_ = "read";
	const string _WRITE_ = "write";
	const string _WHILE_ = "while";
	const string _TO_ = "to";
	const string _DOWNTO_ = "downto";
	const string _DO_ = "do";
	const string _DIV_ = "div";
	const string _MOD_ = "mod";
	const string _OR_ = "or";
	const string _AND_ = "and";
	const string _NOT_ = "not";
	const string _TRUE_ = "true";
	const string _FALSE_ = "false";

	vector<string> newTypes;

	bool result = false;
	bool record = false;
	int mainIndex = 0;
	set <char> setSeparator{ ' ','\t','\r','\n' };
	set <string> setRelations{ "<",">","<>","<=",">=","=" };
	int num_err_str = 1;
	int fix_ind = 0;

	bool err_r_w = false;
	int ind_br = 0;

	bool err_last = false;
	int ind_last = 0;


	bool nextToken() {
		if (mainIndex == 0) {
			if (text[mainIndex] == '\n') {
				num_err_str++;
			}
		}
		if (mainIndex < text.size()) {
			mainIndex++;
			if (text[mainIndex] == '\n') {
				if (mainIndex > fix_ind) {
					num_err_str++;
				}
			}
			return true;
		}
		return false;
	}

	bool checkScapes(int ind) {
		if (text[ind] == ' ') {
			return true;
		}
		return false;
	}

	bool nextTokenSkip() {
		bool fl = nextToken();
		if (fl) {
			while (mainIndex < text.size() && (setSeparator.find(text[mainIndex]) != setSeparator.end())) {
				mainIndex++;
				if (text[mainIndex] == '\n') {
					if (mainIndex > fix_ind) {
						num_err_str++;
					}
				}
			}
		}
		return fl;
	}

	char getToken(bool skipSeparator = false) {
		if (skipSeparator && (setSeparator.find(text[mainIndex]) != setSeparator.end())) {
			nextTokenSkip();
		}
		return text[mainIndex];
	}

	bool setIndex(int to_change) {
		int buf = 0;
		if (to_change < text.size()) {
			if (mainIndex >= fix_ind) {
				fix_ind = mainIndex;
			}
			mainIndex = to_change;
			return true;
		}
		return false;
	}

	bool terminal(string word, bool skip_sep = false) {
		if (skip_sep) {
			getToken(true);
		}
		for (auto i : word) {
			if (i == getToken() && nextToken()) {
			}
			else {
				return false;
			}
		}
		return true;
	}

	bool prog() {
		if (headProgram() && getToken(true) == ';' && nextToken() && block() && last()) {
			return true;
		}
		return false;
	}

	bool last() {
		int index = mainIndex;
		if (getToken(true) == '.') {
			nextToken();
			while (mainIndex < text.size() && (setSeparator.find(text[mainIndex]) != setSeparator.end())) {
				mainIndex++;
			}
			if (mainIndex == text.size()) return true;
			ind_last = mainIndex - 1;
			err_last = true;
			return false;
		}
		ind_last = mainIndex - 1;
		err_last = true;
		return false;
	}

	bool headProgram() {
		if (terminal(_PROGRAM_, true) && nextTokenSkip() && identifier()) {
			return true;
		}
		return false;
	}

	bool block() {
		int index = mainIndex;
		if ((typeSect() && descriptionsSect() && operatorsSect()) || (typeSect() && operatorsSect())
			|| (descriptionsSect() && operatorsSect()) || (setIndex(index) && operatorsSect())) {
			return true;
		}
		return false;
	}

	bool typeSect() {
		if (terminal(_TYPE_, true) && nextTokenSkip() && descriptionsType() && getToken(true) == ';' && nextToken()) {
			int index = mainIndex;
			while (descriptionsVars() && getToken(true) == ';' && nextToken()) {
				index = mainIndex;
			}
			mainIndex = index;
			return true;
		}
		return false;
	}

	bool descriptionsType() {
		if (identifier(true) && getToken(true) == '=' && nextToken() && types("type")) {
			if (record) {
				if (!recordType()) {
					return false;
				}
			}
			return true;
		}
		return false;
	}

	bool endCheck() {
		string buf;
		buf += text[mainIndex];
		buf += text[mainIndex + 1];
		buf += text[mainIndex + 2];
		buf += text[mainIndex + 3];

		if (buf == "end;") {
			return true;
		}
		else
			return false;
	}

	bool recordType() {
		int index;

		while (descriptionsVars("type") && getToken(true) == ';' && nextToken()) {
			index = mainIndex;
		}
		
		return true;
	}

	bool descriptionsSect() {
		int index = mainIndex;
		if (varsSect()) {
			return true;
		}
		return false;
	}

	bool varsSect() {
		if (terminal(_VAR_, true) && nextTokenSkip() && descriptionsVars() && getToken(true) == ';' && nextToken()) {
			int index = mainIndex;
			while (descriptionsVars() && getToken(true) == ';' && nextToken()) {
				index = mainIndex;
			}
			mainIndex = index;
			return true;
		}
		return false;
	}

	bool descriptionsVars(string flag = "var") {
		if (listVarsNames() && getToken(true) == ':' && nextToken() && types(flag)) {
			return true;
		}
		return false;
	}

	bool listVarsNames() {
		if (identifier()) {
			int index = mainIndex;
			while (getToken(true) == ',' && nextToken() && identifier()) {
				index = mainIndex;
			}
			mainIndex = index;
			return true;
		}
		return false;
	}

	bool operatorsSect() {
		if (terminal(_BEGIN_, true) && nextTokenSkip()) {
			int index = mainIndex;
			if ((operatorsList() && terminal(_END_, true)) || (setIndex(index) && terminal(_END_))) {
				return true;
			}
		}
		return false;
	}

	bool operator_() {
		int index = mainIndex;
		if (IOoperator() || (setIndex(index) && ifoperator()) || (setIndex(index) && whileoperator()) ||
			(setIndex(index) && operatorsAssignment()) || (setIndex(index) && compoundOperator())) {
			return true;
		}
		return false;
	}

	bool lstWrite(string state = "write") {
		int index = mainIndex;
		if (setIndex(index)) {
			if (state == "write" && (expr1() || getToken() == ')')) {
				index = mainIndex;
				char parse = getToken(true);
				if (parse != ')') {
					if (parse == ',') {
						nextToken();
						while (parse == ',' && expr1()) {
							index = mainIndex;
							parse = getToken(true);
							nextToken();
						}
						if (parse != ')') return false;
						return true;
					}
					if (expr1()) return true;
					return false;
				}
				nextToken();
				return true;
			}
			else if (state == "read" && (expr2() || getToken() == ')')) {
				index = mainIndex;
				char parse = getToken(true);
				if (parse != ')') {
					if (parse == ',') {
						nextToken();
						while (parse == ',' && expr1()) {
							index = mainIndex;
							parse = getToken(true);
							nextToken();
						}
						if (parse != ')') return false;
						return true;
					}
					if (expr1()) return true;
					return false;
				}
				nextToken();
				return true;
			}
			else 
				return false;
		}
		else 
			return false;
	}

	bool IOoperator() {
		getToken(true);
		int index = mainIndex;
		if (terminal(_WRITE_) && getToken(true) == '(' && nextToken()) {
			ind_br = mainIndex - 1;
			int index = mainIndex;
			if (getToken(true) == ')' && nextToken() || setIndex(index) && lstWrite())
			{
				return true;
			}
			err_r_w = true;
			return false;
		}
		else if (setIndex(index) && terminal(_READ_) && getToken(true) == '(' && nextToken()) {
			ind_br = mainIndex - 1;
			int index = mainIndex;
			if ((getToken(true) == ')' && nextToken()) || (setIndex(index) && lstWrite("read")))
			{
				return true;
			}
			err_r_w = true;
			return false;
		}
		return false;
	}

	bool  ifoperator() {
		if (terminal(_IF_, true) && logExpr() && terminal(_THEN_, true) && operator_()) {
			return true;
		}
		return false;
	}

	bool  whileoperator() {
		if (terminal(_WHILE_, true) && logExpr() && terminal(_DO_, true) && operator_()) {
			return true;
		}
		return false;
	}

	bool  compoundOperator() {
		if (terminal(_BEGIN_, true) && nextTokenSkip()) {
			int index = mainIndex;;
			if ((operatorsList() && terminal(_END_, true)) || (setIndex(index) && terminal(_END_))) {
				return true;
			}
		}
		return false;
	}

	bool  operatorsList() {
		if (operator_() && getToken() == ';') {
			int index = mainIndex;
			bool fl = true;
			while (fl && getToken(true) == ';' && nextToken()) {
				index = mainIndex;
				if (operator_() && getToken() == ';') {
					index = mainIndex;
				}
				else {
					fl = false;
				}
			}
			mainIndex = index;
			return true;
		}
		return false;
	}

	bool  operatorsAssignment() {
		if (var() && terminal(":=", true) && expr()) {
			return true;
		}
		return false;
	}

	bool  expr1() {
		int index = mainIndex;
		if (getToken(true) == ')' || getToken(true) == ',') return false;
		if ((getToken(true) && identifier() && getToken() == '.' && mainIndex++ && identifier())
			|| (setIndex(index) && getToken(true) && var()) 
			|| (getToken(true) && texting()) || (setIndex(index) && getToken(true) && expr())) {
			return true;
		}
		return false;
	}

	bool  expr2() {
		int index = mainIndex;
		if (getToken(true) == ')' || getToken(true) == ',') return false;
		if ((getToken(true) && identifier() && getToken() == '.' && mainIndex++ && identifier())
			|| (setIndex(index) && getToken(true) && var())) {
			return true;
		}
		return false;
	}

	bool texting() {
		char st = getToken(true);
		int index = mainIndex;
		if (st == '\'') {
			nextTokenSkip();
			while (isalpha(getToken()) || isdigit(getToken())) {
				nextTokenSkip();
			}
			if (getToken() == '\'')
			{
				nextTokenSkip();
				return true;
			}
		}
		return false;
	}

	bool  expr() {
		int index = mainIndex;
		if (arithmExpr() || (setIndex(index) && logExpr())) {
			return true;
		}
		return false;
	}

	bool  logExpr() {
		int index = mainIndex;;
		if (relationship() || (setIndex(index) && simpleLogExpr())) {
			return true;
		}
		return false;
	}

	bool  simpleLogExpr() {
		if (logTerm()) {
			int index = mainIndex;
			if (terminal(_OR_, true) && simpleLogExpr()) {
				index = mainIndex;
			}
			mainIndex = index;
			return true;
		}
		return false;
	}

	bool  logTerm() {
		if (logMultiplier()) {
			int index = mainIndex;
			if (terminal(_AND_, true) && logTerm()) {
				index = mainIndex;
			}
			mainIndex = index;
			return true;
		}
		return false;
	}

	bool  logMultiplier() {
		getToken(true);
		int index = mainIndex;
		if (logConst()
			|| (setIndex(index) && terminal(_NOT_) && logMultiplier())
			|| (setIndex(index) && terminal("true"))
			|| (setIndex(index) && getToken(true) == '(' && nextToken() && logExpr() && getToken(true) == ')' && nextToken())
			) {
			return true;
		}
		return false;
	}

	bool  relationship() {
		int index = mainIndex;
		if ((arithmExpr() && OPcomparison() && arithmExpr())
			|| (setIndex(index) && simpleLogExpr() && OPcomparison() && simpleLogExpr())
			) {
			return true;
		}
		return false;
	}

	bool  arithmExpr() {
		int index = mainIndex;
		if (OPaddition()) {
			index = mainIndex;
		}
		else {
			mainIndex = index;
		}
		if (term()) {
			index = mainIndex;
			while (OPaddition() && term()) {
				index = mainIndex;
			}
			mainIndex = index;
			return true;
		}
		return false;
	}

	bool  term() {
		if (multiplier()) {
			int index = mainIndex;
			while (OPmultiplication() && multiplier()) {
				index = mainIndex;
			}
			mainIndex = index;
			return true;
		}
		return false;
	}

	bool  multiplier() {
		getToken(true);
		int index = mainIndex;
		if (intNumber()
			|| (setIndex(index) && var())
			|| (setIndex(index) && getToken(true) == '(' && nextToken() && arithmExpr() && getToken(true) == ')' && nextToken())
			) {
			return true;
		}
		return false;
	}

	bool  var() {
		int index = mainIndex;
		if ((identifier() && getToken() == '.' && mainIndex++ && identifier()) || (setIndex(index) && identifier())) {
			return true;
		}
		return false;
	}

	bool  intNumber() {
		bool firstToken = getToken(true);
		int isNumber = false;
		if (firstToken == '-' || firstToken == '+') {
			nextToken();
		}
		while (isdigit(getToken()) && nextToken()) {
			isNumber = true;
		}
		if (isNumber) {
		}
		return isNumber;
	}

	bool  OPaddition() {
		getToken(true);
		int index = mainIndex;
		if ((getToken() == '+' && nextToken())
			|| (setIndex(index) && (getToken() == '-' && nextToken()))
			) {
			return true;
		}
		return false;
	}

	bool  OPmultiplication() {
		getToken(true);
		int index = mainIndex;
		if ((getToken() == '*' && nextToken())
			|| (setIndex(index) && checkScapes(mainIndex - 1) && terminal(_DIV_) && checkScapes(mainIndex)) || (setIndex(index) && checkScapes(mainIndex - 1) && terminal(_MOD_) && checkScapes(mainIndex)) || (setIndex(index) && (getToken() == '/' && nextToken()))
			) {
			return true;
		}
		return false;
	}

	bool  logConst() {
		getToken(true);
		int index = mainIndex;
		if (terminal(_TRUE_)
			|| (setIndex(index) && terminal(_FALSE_))
			) {
			return true;
		}
		return false;
	}

	bool  OPcomparison() {
		getToken(true);
		int index = mainIndex;
		for (auto i : setRelations) {
			if (setIndex(index) && terminal(i)) {
				return true;
			}
		}
		return false;
	}

	bool  identifier(bool save = false) {
		int index = mainIndex;
		if (save) {
			string buf;
			while (text[mainIndex] != ',' && text[mainIndex] != ' ' && text[mainIndex] != '=') {
				buf += text[mainIndex];
				mainIndex++;
			}
			newTypes.push_back(buf);

			mainIndex = index;
		}

		int dopIndex;
		if (isalpha(getToken(true)) && nextToken()) {
			while ((isalpha(getToken()) || isdigit(getToken())) && nextToken()) {
			}
			dopIndex = mainIndex;
			if (!((setIndex(index) && (terminal(_TRUE_) || terminal(_FALSE_)))))
			{
				mainIndex = dopIndex;
				return true;
			}
			else mainIndex = dopIndex;
		}
		return false;
	}

	bool  types(string block = "var") {
		getToken(true);
		int index = mainIndex;
		string buf;

		while (text[mainIndex] != ' ' && text[mainIndex] != ';') {
			buf += text[mainIndex];
			mainIndex++;
		}
		for (int i = 0; i < newTypes.size(); i++) {
			if (newTypes[i] == buf)
				return true;
		}

		mainIndex = index;

		if (block == "var") {
			if (terminal(_INTEGER_)
				|| (setIndex(index) && terminal(_BOOLEAN_))) {
				return true;
			}
		}
		else {
			if (terminal(_INTEGER_)
				|| (setIndex(index) && terminal(_BOOLEAN_))) {
				return true;
			}
			if (setIndex(index) && terminal(_RECORD_)) {
				record = true;
					return true;
			}
		}

		return false;
	}
};



int main()
{
	setlocale(LC_ALL, "Russian");
	int select;

	cout << "Имя файла: ";
	char name[40];
	cin.getline(name, 40);
	fstream fin;
	fin.open(name, fstream::in);



	if (fin.is_open()) {
		char ch;
		string textOnPascal;
		map <int, vector<int> > mp;
		vector<int> line;
		int num_str = 1;
		int ind = 0;



		while (fin.get(ch))
		{

			if (ch != '\n') {
				line.push_back(ind);
			}
			else {
				line.push_back(ind);
				mp[num_str] = line;
				num_str++;
				line.clear();
			}
			textOnPascal.append(1, ch);
			ind++;
		}
		mp[num_str] = line;

		cout << "\n\n";
		cout << textOnPascal;

		// cout << endl << textOnPascal.find("\0");



		Parser parse;
		parse.CheckProgram(textOnPascal, mp);
	}
	else {
		cout << "Файл не найден";
	}
	return 0;
}