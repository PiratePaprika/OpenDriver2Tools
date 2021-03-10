#ifndef DRAWMODEL_H
#define DRAWMODEL_H

#include "math/Vector.h"
#include "util/DkList.h"

struct RegionModels_t;
struct ModelRef_t;
struct GrVAO;

struct modelBatch_t
{
	int tpage;
	int startIndex;
	int numIndices;
};

class CRenderModel
{
public:
					CRenderModel();
	virtual			~CRenderModel();

	bool			Initialize(ModelRef_t* model);
	void			Destroy();

	void			Draw();

	void			GetExtents(Vector3D& outMin, Vector3D& outMax) const;
	
protected:
	void			GenerateBuffers();

	Vector3D				m_extMin;
	Vector3D				m_extMax;

	ModelRef_t*				m_sourceModel { nullptr };
	GrVAO*					m_vao { nullptr };
	DkList<modelBatch_t>	m_batches;
	int						m_numVerts;
	 
};

#endif