#include "stdafx.h"

void CRenderTarget::phase_ssr()
{
    u32 Offset;
    Fvector2 p0, p1;

    // common 
    struct v_aa
    {
        Fvector4 p;
        Fvector2 uv0;
    };

    float _w = float(Device.dwWidth);
    float _h = float(Device.dwHeight);

    p0.set(.5f / _w, .5f / _h);
    p1.set((_w + .5f) / _w, (_h + .5f) / _h);

    //***SSR GENERATION***
    /*
    In this pass generates SSR
    */
    // Set RT's
    u_setrt(rt_ssr, 0, 0, HW.pBaseZB);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    v_aa* pv = (v_aa*)RCache.Vertex.Lock(4, g_KD->vb_stride, Offset);
    pv->p.set(EPS, float(_h + EPS), EPS, 1.f); pv->uv0.set(p0.x, p1.y); pv++;
    pv->p.set(EPS, EPS, EPS, 1.f); pv->uv0.set(p0.x, p0.y); pv++;
    pv->p.set(float(_w + EPS), float(_h + EPS), EPS, 1.f); pv->uv0.set(p1.x, p1.y); pv++;
    pv->p.set(float(_w + EPS), EPS, EPS, 1.f); pv->uv0.set(p1.x, p0.y); pv++;
    RCache.Vertex.Unlock(4, g_KD->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssr->E[0]);
	//view to world matrix
	Fmatrix m_inv_v;
	m_inv_v.invert(Device.mView);
	RCache.set_c("m_inv_v", m_inv_v);
	
    RCache.set_Geometry(g_KD);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    //***FIRST PASS***
    /*
    first blurring pass
    */
    // Set RT's
    u_setrt(rt_ssr_blur, 0, 0, HW.pBaseZB);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (v_aa*)RCache.Vertex.Lock(4, g_KD->vb_stride, Offset);
    pv->p.set(EPS, float(_h + EPS), EPS, 1.f); pv->uv0.set(p0.x, p1.y); pv++;
    pv->p.set(EPS, EPS, EPS, 1.f); pv->uv0.set(p0.x, p0.y); pv++;
    pv->p.set(float(_w + EPS), float(_h + EPS), EPS, 1.f); pv->uv0.set(p1.x, p1.y); pv++;
    pv->p.set(float(_w + EPS), EPS, EPS, 1.f); pv->uv0.set(p1.x, p0.y); pv++;
    RCache.Vertex.Unlock(4, g_KD->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssr->E[1]);
	RCache.set_c("blur_params", 1.0, 0.0, _w, _h);
    RCache.set_Geometry(g_KD);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    //***SECOND PASS***
    /*
    second blurring pass
    */
    // Set RT's
    u_setrt(rt_ssr, 0, 0, HW.pBaseZB);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (v_aa*)RCache.Vertex.Lock(4, g_KD->vb_stride, Offset);
    pv->p.set(EPS, float(_h + EPS), EPS, 1.f); pv->uv0.set(p0.x, p1.y); pv++;
    pv->p.set(EPS, EPS, EPS, 1.f); pv->uv0.set(p0.x, p0.y); pv++;
    pv->p.set(float(_w + EPS), float(_h + EPS), EPS, 1.f); pv->uv0.set(p1.x, p1.y); pv++;
    pv->p.set(float(_w + EPS), EPS, EPS, 1.f); pv->uv0.set(p1.x, p0.y); pv++;
    RCache.Vertex.Unlock(4, g_KD->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssr->E[2]);
	RCache.set_c("blur_params", 0.0, 1.0, _w, _h);
    RCache.set_Geometry(g_KD);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    //***THIRD PASS***
    /*
    Combine pass
    */
    // Set RT's
    u_setrt(rt_Generic, 0, 0, HW.pBaseZB);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (v_aa*)RCache.Vertex.Lock(4, g_KD->vb_stride, Offset);
    pv->p.set(EPS, float(_h + EPS), EPS, 1.f); pv->uv0.set(p0.x, p1.y); pv++;
    pv->p.set(EPS, EPS, EPS, 1.f); pv->uv0.set(p0.x, p0.y); pv++;
    pv->p.set(float(_w + EPS), float(_h + EPS), EPS, 1.f); pv->uv0.set(p1.x, p1.y); pv++;
    pv->p.set(float(_w + EPS), EPS, EPS, 1.f); pv->uv0.set(p1.x, p0.y); pv++;
    RCache.Vertex.Unlock(4, g_KD->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssr->E[3]);
    RCache.set_Geometry(g_KD);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    RCache.set_Stencil(FALSE);

    HW.pContext->CopyResource(rt_Generic_0->pTexture->surface_get(), rt_Generic->pTexture->surface_get());
};
