#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include <vector>

using namespace std;

struct Token
{
    string text;
    string type;
    int line;
};

bool isOperand(const Token& token)
{
    return token.type == "identifier" || token.type == "digit";
}

bool isOperator(const string& text)
{
    return text == "+" || text == "-" || text == "*" || text == "/";
}

int priority(const string& op)
{
    if (op == "u-")
        return 3;

    if (op == "*" || op == "/")
        return 2;

    if (op == "+" || op == "-")
        return 1;

    return 0;
}

void addToken(string& result, const string& token)
{
    result += token;
    result += ' ';
}

string toPostfix(const vector<Token>& expression)
{
    stack<string> operators;
    string result;
    bool needOperand = true;

    for (int i = 0; i < expression.size(); i++)
    {
        Token token = expression[i];

        if (isOperand(token) || token.type == "wrong-lexem")
        {
            addToken(result, token.text);
            needOperand = false;
        }
        else if (token.text == "(")
        {
            operators.push(token.text);
            needOperand = true;
        }
        else if (token.text == ")")
        {
            while (!operators.empty() && operators.top() != "(")
            {
                addToken(result, operators.top());
                operators.pop();
            }

            if (!operators.empty())
                operators.pop();

            if (!operators.empty() && operators.top() == "u-")
            {
                addToken(result, operators.top());
                operators.pop();
            }

            needOperand = false;
        }
        else if (isOperator(token.text))
        {
            string op = token.text;

            if (op == "-" && needOperand)
                op = "u-";

            while (!operators.empty() && operators.top() != "(" &&
                priority(operators.top()) >= priority(op))
            {
                addToken(result, operators.top());
                operators.pop();
            }

            operators.push(op);
            needOperand = true;
        }
    }

    while (!operators.empty())
    {
        if (operators.top() != "(")
            addToken(result, operators.top());

        operators.pop();
    }

    return result;
}

bool readTokens(vector<Token>& tokens)
{
    ifstream fin("lexemm.txt");

    if (!fin.is_open())
        return false;

    Token token;

    while (fin >> token.text >> token.type >> token.line)
        tokens.push_back(token);

    fin.close();
    return true;
}

int main()
{
    setlocale(LC_ALL, "rus");

    vector<Token> tokens;

    if (!readTokens(tokens))
    {
        cout << "Не удалось открыть lexemm.txt" << endl;
        return 1;
    }

    ofstream fout("result.txt");

    if (!fout.is_open())
    {
        cout << "Не удалось открыть result.txt" << endl;
        return 1;
    }

    for (int i = 0; i < tokens.size(); i++)
    {
        if (tokens[i].type == "identifier" &&
            i + 1 < tokens.size() && tokens[i + 1].text == "=")
        {
            string leftPart = tokens[i].text;
            vector<Token> expression;
            i += 2;

            while (i < tokens.size() && tokens[i].text != ";")
            {
                expression.push_back(tokens[i]);
                i++;
            }

            fout << leftPart << ' ' << toPostfix(expression) << "= ;" << endl;
        }
    }

    fout.close();

    cout << "Постфиксная запись записана в result.txt" << endl;

    return 0;
}

