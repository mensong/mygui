/*!
	@file
	@author		Albert Semenov
	@date		07/2012
*/

#include "Precompiled.h"
#include "StateTextureController.h"
#include "FactoryManager.h"
#include "DataSelectorManager.h"
#include "DataManager.h"
#include "PropertyUtility.h"
#include "ScopeManager.h"
#include "DataUtility.h"

namespace tools
{

	FACTORY_ITEM_ATTRIBUTE(StateTextureController)

	void StateTextureController::setTarget(Control* _control)
	{
		mControl = _control->findControl<ScopeTextureControl>();
	}

	void StateTextureController::activate()
	{
		mParentTypeName = "Skin";
		mThisType = "State";
		mScopeName = "State";

		ScopeManager::getInstance().eventChangeScope.connect(this, &StateTextureController::notifyChangeScope);
		notifyChangeScope(ScopeManager::getInstance().getCurrentScope());
	}

	void StateTextureController::deactivate()
	{
		ScopeManager::getInstance().eventChangeScope.disconnect(this);
	}

	void StateTextureController::notifyChangeDataSelector(DataPtr _data, bool _changeOnlySelection)
	{
		mParentData = _data;
		if (mParentData != nullptr && mParentData->getType()->getName() != mParentTypeName)
			mParentData = nullptr;

		std::string_view texture;
		PropertyPtr property = PropertyUtility::getPropertyByName("Skin", "Texture");
		if (property != nullptr)
		{
			texture = property->getValue();

			if (!property->eventChangeProperty.exist(this, &StateTextureController::notifyChangeProperty))
				property->eventChangeProperty.connect(this, &StateTextureController::notifyChangeProperty);
		}

		std::string_view coord;
		property = PropertyUtility::getPropertyByName("Skin", "Size");
		if (property != nullptr)
		{
			coord = property->getValue();

			if (!property->eventChangeProperty.exist(this, &StateTextureController::notifyChangeProperty))
				property->eventChangeProperty.connect(this, &StateTextureController::notifyChangeProperty);
		}

		if (mParentData != nullptr)
		{
			for (Data::VectorData::const_iterator child = mParentData->getChilds().begin();
				 child != mParentData->getChilds().end();
				 child++)
			{
				if ((*child)->getType()->getName() != mThisType)
					continue;

				property = (*child)->getProperty("Point");
				if (!property->eventChangeProperty.exist(this, &StateTextureController::notifyChangeProperty))
					property->eventChangeProperty.connect(this, &StateTextureController::notifyChangeProperty);

				property = (*child)->getProperty("Visible");
				if (!property->eventChangeProperty.exist(this, &StateTextureController::notifyChangeProperty))
					property->eventChangeProperty.connect(this, &StateTextureController::notifyChangeProperty);
			}
		}

		updateTexture(texture);
		updateCoords(coord);
	}

	void StateTextureController::notifyChangeProperty(PropertyPtr _sender)
	{
		if (!mActivated)
			return;

		if (_sender->getOwner()->getType()->getName() == "Skin")
		{
			if (_sender->getType()->getName() == "Texture")
				updateTexture(_sender->getValue());
			else if (_sender->getType()->getName() == "Size")
				updateCoords(_sender->getValue());
		}
		else if (_sender->getOwner()->getType()->getName() == "State")
		{
			if (_sender->getType()->getName() == "Point")
				updateFrames();
			else if (_sender->getType()->getName() == "Visible")
				updateFrames();
		}
	}

	void StateTextureController::notifyChangeValue(std::string_view _value)
	{
		if (mParentData != nullptr)
		{
			DataPtr selected = mParentData->getChildSelected();
			if (selected != nullptr && selected->getType()->getName() == "State")
			{
				MyGUI::IntCoord coord = MyGUI::IntCoord::parse(_value);
				PropertyPtr property = selected->getProperty("Point");
				PropertyUtility::executeAction(property, coord.point().print(), true);
			}
		}
	}

	void StateTextureController::notifyChangeScope(std::string_view _scope)
	{
		if (mControl == nullptr)
			return;

		if (_scope == mScopeName)
		{
			if (!mActivated)
			{
				mControl->eventChangeValue.connect(this, &StateTextureController::notifyChangeValue);
				mControl->clearAll();

				DataSelectorManager::getInstance()
					.getEvent(mParentTypeName)
					->connect(this, &StateTextureController::notifyChangeDataSelector);
				mParentData = DataUtility::getSelectedDataByType(mParentTypeName);
				notifyChangeDataSelector(mParentData, false);

				mControl->getRoot()->setUserString("CurrentScopeController", mScopeName);

				mActivated = true;
			}
		}
		else
		{
			if (mActivated)
			{
				mControl->eventChangeValue.disconnect(this);

				DataSelectorManager::getInstance().getEvent(mParentTypeName)->disconnect(this);
				mParentData = nullptr;

				// мы еще владельцы контрола сбрасываем его
				std::string_view value = mControl->getRoot()->getUserString("CurrentScopeController");
				if (value == mScopeName)
				{
					mControl->getRoot()->setUserString("CurrentScopeController", std::string_view{});
					notifyChangeDataSelector(mParentData, false);

					mControl->clearAll();
				}

				mActivated = false;
			}
		}
	}

	void StateTextureController::updateCoords(std::string_view _value)
	{
		MyGUI::IntCoord coord;
		if (MyGUI::utility::parseComplex(_value, coord.left, coord.top, coord.width, coord.height))
			mSize = coord.size();
		else
			mSize.clear();

		updateFrames();
	}

	void StateTextureController::updateFrames()
	{
		mFrames.clear();

		if (mParentData != nullptr)
		{
			DataPtr selected = mParentData->getChildSelected();
			for (Data::VectorData::const_iterator child = mParentData->getChilds().begin();
				 child != mParentData->getChilds().end();
				 child++)
			{
				if ((*child)->getType()->getName() != mThisType)
					continue;

				bool visible = (*child)->getPropertyValue<bool>("Visible");
				MyGUI::IntPoint value = (*child)->getPropertyValue<MyGUI::IntPoint>("Point");

				if (selected == *child)
				{
					if (visible)
						mControl->setCoordValue(MyGUI::IntCoord(value, mSize), ScopeTextureControl::SelectorPosition);
					else
						mControl->clearCoordValue();
				}
				else
				{
					if (visible)
						mFrames.emplace_back(MyGUI::IntCoord(value, mSize), ScopeTextureControl::SelectorPosition);
				}
			}

			if (selected == nullptr)
			{
				mControl->clearCoordValue();
			}
		}

		if (mControl != nullptr)
			mControl->setViewSelectors(mFrames);
	}

	void StateTextureController::updateTexture(std::string_view _value)
	{
		mControl->setTextureValue(MyGUI::UString(_value));
		mControl->resetTextureRegion();
	}

}
