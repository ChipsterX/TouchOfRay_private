#pragma once

class CBlender_fxaa : public IBlender
{
public:
    LPCSTR getComment() override { return "fxaa"; }
    BOOL canBeDetailed() override { return FALSE; }
    BOOL canBeLMAPped() override { return FALSE; }

    void Compile(CBlender_Compile& C) override;

    CBlender_fxaa();
    virtual ~CBlender_fxaa();
};
