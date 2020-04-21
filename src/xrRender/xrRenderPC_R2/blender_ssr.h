#pragma once

class CBlender_ssr : public IBlender
{
public:
    LPCSTR getComment() override { return "AAAA MAGIC POHUY"; }
    BOOL canBeDetailed() override { return FALSE; }
    BOOL canBeLMAPped() override { return FALSE; }

    void Compile(CBlender_Compile& C) override;

    CBlender_ssr();
    virtual ~CBlender_ssr();
};
