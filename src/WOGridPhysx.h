#pragma once
#include "WOGridECEFElevation.h"
#include "WONVPhysX.h"
#include <PxPhysicsAPI.h>  
#include <extensions/PxDefaultErrorCallback.h>
#include <extensions/PxDefaultAllocator.h>
#include "WOPhysX.h"
#include <vector>
namespace Aftr
{
	class WOGridPhysx : public WOGrid
	{
	public:
		//psc  
		static WOGridPhysx* New(physx::PxFoundation* foundation,
			physx::PxPhysics* physics, physx::PxScene* scene, std::vector<std::vector<VectorD>>& myGrid, const VectorD& scale = VectorD(1, 1, 1), const std::vector< std::vector< aftrColor4ub > >& colors = std::vector< std::vector< aftrColor4ub > >());
		virtual ~WOGridPhysx() = default;
	protected:
		WOGridPhysx();
		virtual void onCreate(physx::PxFoundation* foundation, physx::PxPhysics* physics, physx::PxScene* scene, std::vector<std::vector<VectorD>>& myGrid, const VectorD& scale, const std::vector< std::vector< aftrColor4ub > >& colors);


	private:
		std::vector<float> vertexListCopy;
		std::vector<unsigned int> indexListCopy;
	};
}

