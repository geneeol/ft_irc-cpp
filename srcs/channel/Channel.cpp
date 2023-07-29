#include "Channel.hpp"

Channel::Channel(const std::string &name) : mName(name), mTopic(std::string()), mModeFlags(0) {}

void Channel::BroadcastMessage(const std::string &message)
{
    std::vector<Client *>::iterator iter = mClients.begin();

    while (iter != mClients.end())
    {
        (*iter)->InsertResponse(message);
        iter++;
    }
}

bool Channel::CheckClientIsExist(const std::string &nickName)
{
    std::vector<Client *>::iterator iter = mClients.begin();

    while (iter != mClients.end())
    {
        if ((*iter)->GetNickName() == nickName)
            return true;

        iter++;
    }

    return false;
}

bool Channel::CheckClientIsOperator(const std::string &nickName)
{
    std::vector<Client *>::iterator iter = mOperators.begin();

    while (iter != mOperators.end())
    {
        if ((*iter)->GetNickName() == nickName)
            return true;

        iter++;
    }

    return false;
}

void Channel::SetClient(Client *newClient)
{
    mClients.push_back(newClient);
}

void Channel::RemoveClient(const std::string &nickName)
{
    if (CheckClientIsOperator(nickName))
        RemoveOperator(nickName);

    std::vector<Client *>::iterator iter;

    iter = mClients.begin();
    while (iter != mClients.end())
    {
        if ((*iter)->GetNickName() == nickName)
        {
            mClients.erase(iter);
            break;
        }

        iter++;
    }
}

void Channel::SetOperator(Client *newOperator)
{
    mOperators.push_back(newOperator);
}

void Channel::RemoveOperator(const std::string &nickName)
{
    std::vector<Client *>::iterator iter;

    iter = mOperators.begin();
    while (iter != mOperators.end())
    {
        if ((*iter)->GetNickName() == nickName)
        {
            mOperators.erase(iter);
            break;
        }

        iter++;
    }
}

const std::string &Channel::GetTopic() const
{
    return mTopic;
}

void Channel::SetTopic(const std::string &topic)
{
    mTopic = topic;
}

bool Channel::IsInviteOnlyMode() const
{
    return (mModeFlags & INVITE_ONLY_FLAG) != 0;
}

bool Channel::IsProtectedTopicMode() const
{
    return (mModeFlags & PROTECTED_TOPIC_FLAG) != 0;
}

bool Channel::IsKeyMode() const
{
    return (mModeFlags & KEY_FLAG) != 0;
}

bool Channel::IsClientLimitMode() const
{
    return (mModeFlags & CLIENT_LIMIT_FLAG) != 0;
}

void Channel::ToggleInviteOnlyMode()
{
    mModeFlags ^= INVITE_ONLY_FLAG;
}

void Channel::ToggleProtectedTopicMode()
{
    mModeFlags ^= PROTECTED_TOPIC_FLAG;
}

void Channel::ToggleKeyMode()
{
    mModeFlags ^= KEY_FLAG;
}

void Channel::ToggleClientLimitMode()
{
    mModeFlags ^= CLIENT_LIMIT_FLAG;
}
