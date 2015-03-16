#ifndef PERMISSIONS_H
#define PERMISSIONS_H


class Permissions
{
public:
    Permissions();
    ~Permissions();

    enum AccessLevel
    {
        Admin = 0xFF,
        Default = 0,
        Implementer = 0x19,
        Moderator = 30,
        Player = 1,
        TeamMember = 20
    };
};

#endif // PERMISSIONS_H
