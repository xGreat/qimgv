#include "thumbnaillabel.h"

ThumbnailLabel::ThumbnailLabel(QGraphicsPixmapItem *parent) :
    QGraphicsLayoutItem(),
    QGraphicsPixmapItem(parent),
    hovered(false),
    loaded(false),
    showLabel(false),
    showName(true),
    state(EMPTY),
    highlighted(false),
    borderW(3),
    borderH(5)
{
    thumbnailSize = globalSettings->thumbnailSize();
    setGraphicsItem(this);
    this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    this->setOffset(QPointF(borderW, borderH));
    this->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);

    highlightRect.setTopLeft(QPointF(borderW, 0));
    highlightRect.setBottomRight(QPointF(borderW+thumbnailSize, borderH));

    nameRect.setTopLeft(QPointF(borderW, borderH));
    nameRect.setBottomRight(QPointF(borderW+thumbnailSize, borderH+16));
    nameRect.setWidth(thumbnailSize);

    shadowRect.setTopLeft(QPointF(borderW, borderH));
    shadowRect.setBottomRight(QPointF(borderW+thumbnailSize, borderH+nameRect.height()));

    labelRect = QRectF(QPointF(borderW, borderH),
                       QPointF(borderW+24, borderH+nameRect.height()));

    //highlightColor = new QColor(48, 140, 171);
    highlightColor = new QColor(globalSettings->accentColor());
    highlightColorBorder = new QColor(20, 26, 17); //unused for now
    outlineColor = new QColor(Qt::black);
    nameColor = new QColor(10, 10, 10, 230);
    labelColor = new QColor(globalSettings->accentColor());
    labelColor->setAlpha(230);
    shadowGradient = new QLinearGradient(shadowRect.topLeft(),
                                         shadowRect.bottomLeft());
    shadowGradient->setColorAt(0, QColor(0,0,0,100));
    shadowGradient->setColorAt(1, QColor(0,0,0,0));
    font.setPixelSize(11);
    fm = new QFontMetrics(font);
}

void ThumbnailLabel::setThumbnail(const Thumbnail *_thumbnail){
    thumbnail = _thumbnail;
    QGraphicsPixmapItem::setPixmap(*thumbnail->image);
    loaded = true;
    showLabel = globalSettings->showThumbnailLabels() && !thumbnail->label.isEmpty();
    if(showLabel) {
        int labelWidth = fm->width(thumbnail->label);
        labelRect.setWidth(labelWidth + 4);
        labelRect.moveRight(nameRect.right());
    }
}

void ThumbnailLabel::setHighlighted(bool x) {
    bool toRepaint = (highlighted==x);
    highlighted = x;
    if(toRepaint) {
        this->update(boundingRect());
    }
}

bool ThumbnailLabel::isHighlighted() {
    return highlighted;
}

void ThumbnailLabel::setOpacityAnimated(qreal amount, int speed) {
    if(speed == 0.0) {
        setOpacity(amount);
        return;
    }
    QPropertyAnimation *anim = new QPropertyAnimation(this, "opacity");
    anim->setDuration(speed);
    anim->setStartValue(this->opacity());
    anim->setEndValue(amount);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
void ThumbnailLabel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    //thumbnail pixmap
    QGraphicsPixmapItem::paint(painter,option,widget);

    //colored bar and shadow on the top
    if(isHighlighted()) {
        painter->fillRect(highlightRect, *highlightColor);
        //painter->fillRect(shadowRect, *shadowGradient);
    }

    //setup font
    painter->setFont(font);

    //nameLabel
    painter->fillRect(nameRect, *nameColor);
    painter->setPen(QColor(10,10,10,200));
    painter->drawText(nameRect.adjusted(3,2,0,0), thumbnail->name);
    painter->setPen(QColor(255,255,255,255));
    painter->drawText(nameRect.adjusted(2,1,0,0), thumbnail->name);

    //typeLabel
    if(showLabel) {
        painter->fillRect(labelRect, *labelColor);
        QPointF labelTextPos = labelRect.bottomLeft()+QPointF(2,-5);
        painter->setPen(QColor(10,10,10,200));
        painter->drawText(labelTextPos+QPointF(1,1), thumbnail->label);
        painter->setPen(QColor(255,255,255,255));
        painter->drawText(labelTextPos, thumbnail->label);
    }
}

QSizeF ThumbnailLabel::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    switch (which) {
    case Qt::MinimumSize:
    case Qt::PreferredSize:
        return QSize(thumbnailSize, thumbnailSize)
               + QSize(borderW*2, borderH*2);
    case Qt::MaximumSize:
        return QSize(thumbnailSize, thumbnailSize)
               + QSize(borderW*2, borderH*2);
    default:
        break;
    }
    return constraint;
}

void ThumbnailLabel::setGeometry(const QRectF &rect) {
    prepareGeometryChange();
    QGraphicsLayoutItem::setGeometry(rect);
    setPos(rect.topLeft());
}

QRectF ThumbnailLabel::boundingRect() const
{
    return QRectF(QPointF(0,0), geometry().size());
}

ThumbnailLabel::~ThumbnailLabel() {

}
