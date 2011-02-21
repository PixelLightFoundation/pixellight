/*********************************************************\
 *  File: SNSystem.cpp                                   *
 *
 *  Copyright (C) 2002-2011 The PixelLight Team (http://www.pixellight.org/)
 *
 *  This file is part of PixelLight.
 *
 *  PixelLight is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  PixelLight is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with PixelLight. If not, see <http://www.gnu.org/licenses/>.
\*********************************************************/


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <PLGeneral/Tools/Timing.h>
#include <PLMath/Math.h>
#include <PLRenderer/RendererContext.h>
#include <PLRenderer/Renderer/FixedFunctions.h>
#include <PLRenderer/Effect/EffectManager.h>
#include <PLScene/Scene/SceneContext.h>
#include <PLScene/Visibility/VisNode.h>
PL_WARNING_PUSH
PL_WARNING_DISABLE(4530) // "warning C4530: C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc"
	#include <SPK.h>
PL_WARNING_POP
#include "SPARK_PL/SNSystem.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLGeneral;
using namespace PLMath;
using namespace PLRenderer;
using namespace PLScene;
namespace SPARK_PL {


//[-------------------------------------------------------]
//[ RTTI interface                                        ]
//[-------------------------------------------------------]
pl_implement_class(SNSystem)


//[-------------------------------------------------------]
//[ Public functions                                      ]
//[-------------------------------------------------------]
/**
*  @brief
*    Default constructor
*/
SNSystem::SNSystem() :
	Flags(this),
	EventHandlerUpdate(&SNSystem::NotifyUpdate, this),
	m_pParticleSystem(nullptr),
	m_bUpdate(false)
{
	// Set draw function flags
	SetDrawFunctionFlags(uint8(GetDrawFunctionFlags() | UseDrawTransparent));
}

/**
*  @brief
*    Destructor
*/
SNSystem::~SNSystem()
{
	// Cleanup
	if (m_pParticleSystem)
		delete m_pParticleSystem;
}


//[-------------------------------------------------------]
//[ Public virtual PLScene::SceneNode functions           ]
//[-------------------------------------------------------]
void SNSystem::DrawTransparent(Renderer &cRenderer, const VisNode *pVisNode)
{
	if (m_pParticleSystem) {
		// Get the fixed functions interface
		FixedFunctions *pFixedFunctions = cRenderer.GetFixedFunctions();
		if (pFixedFunctions) {
			cRenderer.GetRendererContext().GetEffectManager().Use();

			// Set the current world matrix
			pFixedFunctions->SetTransformState(FixedFunctions::Transform::World, pVisNode->GetWorldMatrix());

			// Draw the SPARK particle system
			m_pParticleSystem->render();
		}
	}
}


//[-------------------------------------------------------]
//[ Protected virtual SNSystem functions                  ]
//[-------------------------------------------------------]
/**
*  @brief
*    Called just before the particle system is updated
*/
void SNSystem::Update()
{
	// The default implementation is empty
}


//[-------------------------------------------------------]
//[ Private virtual PLScene::SceneNode functions          ]
//[-------------------------------------------------------]
void SNSystem::OnActivate(bool bActivate)
{
	// Call the base implementation
	SceneNode::OnActivate(bActivate);

	// Connect/disconnect event handler
	SceneContext *pSceneContext = GetSceneContext();
	if (pSceneContext) {
		if (bActivate)
			pSceneContext->EventUpdate.Connect(&EventHandlerUpdate);
		else
			pSceneContext->EventUpdate.Disconnect(&EventHandlerUpdate);
	}
}

void SNSystem::UpdateAABoundingBox()
{
	// Is the computation of the axis aligned bounding box enabled?
	if (m_pParticleSystem && m_pParticleSystem->isAABBComputingEnabled() && !(GetFlags() & NoAutomaticAABB)) {
		// Get the minimum/maximum of the axis aligned bounding box (in 'scene node space')
		const SPK::Vector3D &vAABBMin = m_pParticleSystem->getAABBMin();
		const SPK::Vector3D &vAABBMax = m_pParticleSystem->getAABBMax();

		// Set the new axis aligned bounding box
		SetAABoundingBox(AABoundingBox(vAABBMin.x, vAABBMin.y, vAABBMin.z, vAABBMax.x, vAABBMax.y, vAABBMax.z));
	}
}

void SNSystem::OnAddedToVisibilityTree(VisNode &cVisNode)
{
	m_bUpdate = true;
}


//[-------------------------------------------------------]
//[ Private functions                                     ]
//[-------------------------------------------------------]
/**
*  @brief
*    Called when the scene node needs to be updated
*/
void SNSystem::NotifyUpdate()
{
	// If this scene node wasn't drawn at the last frame, we can skip some update stuff
	if (m_pParticleSystem && ((GetFlags() & UpdateInvisible) || m_bUpdate)) {
		m_bUpdate = false;

		// Give the derived systems an update change
		Update();

		// Get the current time difference
		const float fTimeDiff = Timing::GetInstance()->GetTimeDifference();

		// Update the SPARK particle system
		m_pParticleSystem->update(fTimeDiff);

		// We have to recalculate the current axis align bounding box in 'scene node space'
		if (m_pParticleSystem->isAABBComputingEnabled() && !(GetFlags() & NoAutomaticAABB))
			DirtyAABoundingBox();
	}
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // SPARK_PL