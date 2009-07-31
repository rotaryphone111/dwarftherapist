/*
Dwarf Therapist
Copyright (c) 2009 Trey Stout (chmod)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include <QtGui>
#include "uberdelegate.h"
#include "dwarfmodel.h"
#include "dwarf.h"
#include "gridview.h"
#include "columntypes.h"

UberDelegate::UberDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{}

void UberDelegate::paint(QPainter *p, const QStyleOptionViewItem &opt, const QModelIndex &proxy_idx) const {
	if (!proxy_idx.isValid()) {
		return;
	}
	if (proxy_idx.column() == 0) { // we never do anything with the 0 col...
		QStyledItemDelegate::paint(p, opt, proxy_idx);
		return;
	}

	paint_cell(p, opt, proxy_idx);
	/*
	QModelIndex model_idx = m_proxy->mapToSource(proxy_idx);
	if (m_model->current_grouping() == DwarfModel::GB_NOTHING) {
		paint_labor(p, opt, model_idx);
	} else {
		QModelIndex first_col = m_model->index(model_idx.row(), 0, model_idx.parent());
		if (m_model->hasChildren(first_col)) { // skill item (under a group header)
			paint_aggregate(p, opt, proxy_idx);
		} else {
			paint_labor(p, opt, model_idx);
		}
	}
	*/
	if (proxy_idx.column() == m_model->selected_col()) {
		p->save();
		p->setPen(QPen(color_guides));
		p->drawLine(opt.rect.topLeft(), opt.rect.bottomLeft());
		p->drawLine(opt.rect.topRight(), opt.rect.bottomRight());
		p->restore();
	}
}

void UberDelegate::paint_cell(QPainter *p, const QStyleOptionViewItem &opt, const QModelIndex &idx) const {
	QModelIndex model_idx = m_proxy->mapToSource(idx);
	COLUMN_TYPE type = static_cast<COLUMN_TYPE>(model_idx.data(DwarfModel::DR_COL_TYPE).toInt());
	switch (type) {
		case CT_LABOR:
			{
				bool agg = model_idx.data(DwarfModel::DR_IS_AGGREGATE).toBool();
				if (m_model->current_grouping() == DwarfModel::GB_NOTHING || !agg) {
					paint_labor(p, opt, idx);
				} else {
					paint_aggregate(p, opt, idx);
				}
			}
			break;
		case CT_HAPPINESS:
			QStyledItemDelegate::paint(p, opt, idx);
			paint_grid(false, p, opt, idx);
			break;
		case CT_DEFAULT:
		case CT_SPACER:
		default:
			QStyledItemDelegate::paint(p, opt, idx);
			break;
	}
}

void UberDelegate::paint_labor(QPainter *p, const QStyleOptionViewItem &opt, const QModelIndex &proxy_idx) const {
	QModelIndex idx = m_proxy->mapToSource(proxy_idx);
	short rating = idx.data(DwarfModel::DR_RATING).toInt();
	
	Dwarf *d = m_model->get_dwarf_by_id(idx.data(DwarfModel::DR_ID).toInt());
	if (!d) {
		return QStyledItemDelegate::paint(p, opt, idx);
	}

	bool skip_border = false;
	int labor_id = idx.data(DwarfModel::DR_LABOR_ID).toInt();
	bool enabled = d->is_labor_enabled(labor_id);
	bool dirty = d->is_labor_state_dirty(labor_id);

	p->save();
	p->fillRect(opt.rect, QBrush(idx.data(DwarfModel::DR_DEFAULT_BG_COLOR).value<QColor>()));
	if (enabled) {
		p->fillRect(opt.rect.adjusted(1, 1, -2, -2), QBrush(color_active_labor));
		//m_model->setData(idx, color_active_labor, Qt::BackgroundColorRole);
		//m_model->setData(idx, idx.data(DwarfModel::DR_DEFAULT_BG_COLOR).value<QColor>(), Qt::BackgroundColorRole);
	}
	//QStyledItemDelegate::paint(p, opt, idx);
	p->restore();
	
	// draw rating
	if (rating == 15) {
		// draw diamond
		p->save();
		p->setRenderHint(QPainter::Antialiasing);
		p->setPen(Qt::gray);
		p->setBrush(QBrush(Qt::red));

		QPolygonF shape;
		shape << QPointF(0.5, 0.1) //top
			<< QPointF(0.75, 0.5) // right
			<< QPointF(0.5, 0.9) //bottom
			<< QPointF(0.25, 0.5); // left

		p->translate(opt.rect.x() + 2, opt.rect.y() + 2);
		p->scale(opt.rect.width()-4, opt.rect.height()-4);
		p->drawPolygon(shape);
		p->restore();

		p->save();
		p->setPen(QPen(QColor(Qt::black), 1));
		p->drawRect(opt.rect.adjusted(1, 1, -2, -2));
		p->restore();
		skip_border = !dirty;

	} else if (rating < 15 && rating > 10) {
		// TODO: try drawing the square of increasing size...
		float size = 0.65f * (rating / 10.0f);
		float inset = (1.0f - size) / 2.0f;

		p->save();
		p->translate(opt.rect.x(), opt.rect.y());
		p->scale(opt.rect.width(), opt.rect.height());
		p->fillRect(QRectF(inset, inset, size, size), QBrush(QColor(0x888888)));
		p->restore();
	} else if (rating > 0) {
		float size = 0.65f * (rating / 10.0f);
		float inset = (1.0f - size) / 2.0f;

		p->save();
		p->translate(opt.rect.x(), opt.rect.y());
		p->scale(opt.rect.width(), opt.rect.height());
		p->fillRect(QRectF(inset, inset, size, size), QBrush(QColor(0xAAAAAA)));
		p->restore();
	}
	if (!skip_border)
		paint_grid(dirty, p, opt, proxy_idx);
}

void UberDelegate::paint_aggregate(QPainter *p, const QStyleOptionViewItem &opt, const QModelIndex &proxy_idx) const {
	if (!proxy_idx.isValid()) {
		return;
	}
	QModelIndex model_idx = m_proxy->mapToSource(proxy_idx);
	QModelIndex first_col = m_proxy->index(proxy_idx.row(), 0, proxy_idx.parent());
	if (!first_col.isValid()) {
		return;
	}
	
	QString group_name = proxy_idx.data(DwarfModel::DR_GROUP_NAME).toString();
	int labor_id = proxy_idx.data(DwarfModel::DR_LABOR_ID).toInt();

	int dirty_count = 0;
	int enabled_count = 0;
	for (int i = 0; i < m_proxy->rowCount(first_col); ++i) {
		int dwarf_id = m_proxy->data(m_proxy->index(i, 0, first_col), DwarfModel::DR_ID).toInt();
		Dwarf *d = m_model->get_dwarf_by_id(dwarf_id);
		if (d && d->is_labor_enabled(labor_id))
			enabled_count++;
		if (d && d->is_labor_state_dirty(labor_id))
			dirty_count++;
	}
	
	QStyledItemDelegate::paint(p, opt, proxy_idx); // always lay the "base coat"
	
	p->save();
	QRect adj = opt.rect.adjusted(1, 1, 0, 0);
	if (enabled_count == m_proxy->rowCount(first_col)) {
		p->fillRect(adj, QBrush(color_active_group));
	} else if (enabled_count > 0) {
		p->fillRect(adj, QBrush(color_partial_group));
	} else {
		p->fillRect(adj, QBrush(color_inactive_group));
	}
	p->restore();

	p->save(); // border last
	p->setBrush(Qt::NoBrush);
	if (dirty_count) {
		p->setPen(QPen(color_dirty_border, 1));
		p->drawRect(opt.rect.adjusted(0, 0, -1, -1));
	} else {
		p->setPen(QPen(color_border));
		p->drawRect(opt.rect);
	}
	p->restore();
}

void UberDelegate::paint_grid(bool dirty, QPainter *p, const QStyleOptionViewItem &opt, const QModelIndex &proxy_idx) const {
	QRect r = opt.rect.adjusted(1, 1, -2, -2);
	p->save(); // border last
	p->setBrush(Qt::NoBrush);
	p->setPen(color_border);
	p->drawRect(r);
	if (opt.state & QStyle::State_Selected) {
		p->setPen(color_guides);
		p->drawLine(opt.rect.topLeft(), opt.rect.topRight());
		p->drawLine(opt.rect.bottomLeft(), opt.rect.bottomRight());
	}
	if (dirty) {
		p->setPen(QPen(color_dirty_border, 1));
		p->drawRect(r);
	}
	p->restore();
}

/* If grid-sizing ever comes back...
QSize UberDelegate::sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &idx) const {
	if (idx.column() == 0)
		return QStyledItemDelegate::sizeHint(opt, idx);
	return QSize(24, 24);
}
*/