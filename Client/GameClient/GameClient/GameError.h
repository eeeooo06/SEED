#ifndef _GAMEERROR_H                                                         // 이 파일이 두 번 이상 포함될 경우 다중 정의를 방지합니다.
#define _GAMEERROR_H            
#define WIN32_LEAN_AND_MEAN

#include <string>
#include <exception>

namespace gameErrorNS
{
    // 오류 코드
    // 음수는 게임을 종료해야 할 수 있는 심각한 오류입니다.
    // 양수는 게임을 종료할 필요가 없는 경고입니다.
    const int FATAL_ERROR = -1;
    const int WARNING = 1;
}

// 게임 오류 클래스. 게임 엔진에 의해 오류가 감지되면 발생합니다.
// std::exception으로부터 상속받습니다.
class GameError : public std::exception
{
private:
    int     errorCode;
    std::string message;
public:
    // 기본 생성자
    GameError() throw() :errorCode(gameErrorNS::FATAL_ERROR), message("Undefined Error in game.") {}
    // 복사 생성자
    GameError(const GameError& e) throw() : std::exception(e), errorCode(e.errorCode), message(e.message) {}
    // 인수가 있는 생성자
    GameError(int code, const std::string& s) throw() :errorCode(code), message(s) {}
    // 할당 연산자
    GameError& operator= (const GameError& rhs) throw()
    {
        std::exception::operator=(rhs);
        this->errorCode = rhs.errorCode;
        this->message = rhs.message;
    }
    // 소멸자
    virtual ~GameError() throw() {};

    // 기본 클래스의 what을 재정의합니다.
    virtual const char* what() const throw() { return this->getMessage(); }

    const char* getMessage() const throw() { return message.c_str(); }
    int getErrorCode() const throw() { return errorCode; }
};

#endif