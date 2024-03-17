#pragma once
#include <QString>

class IHandler
{
public:
    virtual ~IHandler() = default;
    virtual bool specHandle(QString& str) = 0;
protected:
    QString query;
};

class SignUpHandler : public IHandler
{
public:
    bool specHandle(QString& str) override;
};

class SignInHandler : public IHandler
{
public:
    bool specHandle(QString& str) override;
};

class PostHandler : public IHandler
{
public:
    bool specHandle(QString& str) override;
};

class UsersDisplayHandler : public IHandler
{
public:
    bool specHandle(QString& str) override;
};
