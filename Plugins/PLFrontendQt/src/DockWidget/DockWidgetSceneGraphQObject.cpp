/*********************************************************\
 *  File: DockWidgetSceneGraphQObject.cpp                *
 *
 *  Copyright (C) 2002-2012 The PixelLight Team (http://www.pixellight.org/)
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
#include "PLFrontendQt/DataModels/SceneGraphTreeModel.h"
#include "PLFrontendQt/DataModels/TreeSortAndFilterProxyModel.h"
#include "PLFrontendQt/DockWidget/DockWidgetSceneGraph.h"
#include "PLFrontendQt/DockWidget/DockWidgetSceneGraphQObject.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLCore;
namespace PLFrontendQt {


//[-------------------------------------------------------]
//[ Private functions                                     ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor
*/
DockWidgetSceneGraphQObject::DockWidgetSceneGraphQObject(DockWidgetSceneGraph &cDockWidgetSceneGraph) :
	m_pDockWidgetSceneGraph(&cDockWidgetSceneGraph)
{
}

/**
*  @brief
*    Destructor
*/
DockWidgetSceneGraphQObject::~DockWidgetSceneGraphQObject()
{
}


//[-------------------------------------------------------]
//[ Private Qt slots (MOC)                                ]
//[-------------------------------------------------------]
void DockWidgetSceneGraphQObject::QtSlotTreeViewDoubleClicked(const QModelIndex &cQModelIndex)
{
	// Is there a scene graph tree model instance?
	DataModels::SceneGraphTreeModel *pSceneGraphTreeModel = m_pDockWidgetSceneGraph->m_pSceneGraphTreeModel;
	DataModels::TreeSortAndFilterProxyModel *pFilterModel = m_pDockWidgetSceneGraph->m_pSortAndFilterModel;
	if (pSceneGraphTreeModel && pFilterModel) {
		// We have been provided with a filter model index, what we need is an index which can be used within the original tree view
		const QModelIndex cQModelIndexSource = pFilterModel->mapToSource(cQModelIndex);

		// Get selected scene node
		Object *pObject = reinterpret_cast<Object*>(pSceneGraphTreeModel->GetSceneNodeFromIndex(cQModelIndexSource));
		if (!pObject) {
			// Hm, maybe it's an selected scene node modifier?
			pObject = reinterpret_cast<Object*>(pSceneGraphTreeModel->GetSceneNodeModifierFromIndex(cQModelIndexSource));
		}

		// Perform a dock widget manager broadcast (excludes this emitting dock widget)
		m_pDockWidgetSceneGraph->CallDockWidgetsMethod("SelectObject", Params<void, Object*>(pObject));
	}
}

void DockWidgetSceneGraphQObject::QtSlotFilterTypeChanged(int filterId)
{
	m_pDockWidgetSceneGraph->m_pSortAndFilterModel->setFilterRole((filterId == ByClassName) ? DataModels::SceneGraphTreeModel::ClassNameRole : Qt::DisplayRole);
}

void DockWidgetSceneGraphQObject::QtSlotFilterChanged(const QString &text)
{
	m_pDockWidgetSceneGraph->m_pSortAndFilterModel->setFilterWildcard(text);
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLFrontendQt