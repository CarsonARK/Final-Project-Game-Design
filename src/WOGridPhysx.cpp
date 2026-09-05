#include "WOGridPhysx.h"
#include "Model.h"
#include "ModelDataShared.h"
#include <cooking/PxCooking.h>  
#include <pvd/PxPvd.h>
#include <pvd/PxPvdSceneClient.h>
#include <iostream>
#include "PxShape.h" // make sure this is present in your file

namespace Aftr
{
	WOGridPhysx* WOGridPhysx::New(physx::PxFoundation* foundation,
		physx::PxPhysics* physics, physx::PxScene* scene, std::vector<std::vector<VectorD>>& myGrid, const VectorD& scale, const std::vector< std::vector< aftrColor4ub > >& colors)
	{
		WOGridPhysx* ret = new WOGridPhysx();
		ret->onCreate(foundation, physics, scene, myGrid, scale, colors);
		return ret;
	}

	WOGridPhysx::WOGridPhysx() : IFace((WO*)this), WOGrid()
	{
	}
	void WOGridPhysx::onCreate(physx::PxFoundation* foundation,
		physx::PxPhysics* physics, physx::PxScene* scene, std::vector<std::vector<VectorD>>& myGrid, const VectorD& scale, const std::vector< std::vector< aftrColor4ub > >& colors)
	{
		WOGrid::onCreate(myGrid, scale, colors);

				size_t vertexListSize = this->getModel()->getModelDataShared()->getCompositeVertexList().size();
				size_t indexListSize = this->getModel()->getModelDataShared()->getCompositeIndexList().size();

				vertexListCopy = std::vector<float>(vertexListSize * 3);
				indexListCopy = std::vector<unsigned int>(indexListSize);

				for (size_t i = 0; i < vertexListSize; ++i)
				{
					vertexListCopy[i * 3] = this->getModel()->getModelDataShared()->getCompositeVertexList()[i].x;
					vertexListCopy[i * 3 + 1] = this->getModel()->getModelDataShared()->getCompositeVertexList()[i].y;
					vertexListCopy[i * 3 + 2] = this->getModel()->getModelDataShared()->getCompositeVertexList()[i].z;
				}
				for (size_t i = 0; i < indexListSize; ++i)
				{
					indexListCopy[i] = this->getModel()->getModelDataShared()->getCompositeIndexList()[i];
				}

				physx::PxTriangleMeshDesc meshDesc;
				meshDesc.points.count = vertexListSize;
				meshDesc.points.stride = sizeof(float) * 3;
				meshDesc.points.data = vertexListCopy.data();

				meshDesc.triangles.count = indexListSize / 3;
				meshDesc.triangles.stride = 3 * sizeof(unsigned int);
				meshDesc.triangles.data = indexListCopy.data();

				physx::PxDefaultMemoryOutputStream writeBuffer;
				physx::PxTriangleMeshCookingResult::Enum result;
				physx::PxTolerancesScale pxScale;
				physx::PxCookingParams params(pxScale);


				meshDesc.flags = physx::PxMeshFlags();
				meshDesc.flags |= physx::PxMeshFlag::eFLIPNORMALS;

				params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eWELD_VERTICES; // Good for stability
				// You can try flipping normals here if the geometry is inside out:
				// meshDesc.flags |= physx::PxMeshFlag::eFLIPNORMALS;
				bool status = PxCookTriangleMesh(params, meshDesc, writeBuffer, &result);
				if (!status) {
					std::cout << "Failed to create Triangular mesh" << std::endl;
					std::cin.get();
				}


				//	params.midphaseDesc.mBVHStructure = physx::PxBVHStructure::eBVH34; // Use the most modern spatial tree
				//params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eWELD_VERTICES;
				//params.meshWeldTolerance = 0.001f;

				// Re-cook with these params



				physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
				physx::PxTriangleMesh* mesh = physics->createTriangleMesh(readBuffer);

				physx::PxMaterial* gMaterial = physics->createMaterial(1.f, 1.f, 0.6f);

				physx::PxTriangleMeshGeometry geom(mesh);

				// Set the flag on the geometry object
				geom.meshFlags |= physx::PxMeshGeometryFlag::eDOUBLE_SIDED;
				physx::PxShape* shape = physics->createShape(geom, *gMaterial, true);
				// Get the position and orientation from the Aftr World Object
				physx::PxTransform t(
					physx::PxVec3(this->getPosition().x, this->getPosition().y, this->getPosition().z)
				);

				std::cout << "POSIITION " << this->getPosition().toString() << std::endl;
				//physx::PxRigidStatic* a = physics->createRigidStatic(t);

				physx::PxRigidStatic* a = physics->createRigidStatic(t);

				// 1. Enable visualization on the specific actor
				a->setActorFlag(physx::PxActorFlag::eVISUALIZATION, true);

				// 2. Enable visualization on the shape
				if (shape)
				{
					// Enable visualization on this shape
					shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);
				}

				physx::PxFilterData filterData;
				filterData.word0 = 1; // My identity (Group 1)
				filterData.word1 = 1; // Who I collide with (Group 1)
				shape->setSimulationFilterData(filterData);
				// 3. Set the scale of the visualization in the scene
				scene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);

				// 4. Specifically ask for Collision Shapes (the wireframe)
				scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

				a->attachShape(*shape);

				a->userData = this;
				scene->addActor(*a);
			//});
	}
}