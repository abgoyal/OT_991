

#ifndef SVGList_h
#define SVGList_h

#if ENABLE(SVG)
#include "ExceptionCode.h"
#include "SVGListTraits.h"

#include <wtf/RefCounted.h>
#include <wtf/PassRefPtr.h>
#include <wtf/Vector.h>

namespace WebCore {

    class QualifiedName;

    template<typename Item>
    struct SVGListTypeOperations {
        static Item nullItem()
        {
            return SVGListTraits<UsesDefaultInitializer<Item>::value, Item>::nullItem();
        }
        static bool isNull(const Item& it)
        {
            return SVGListTraits<UsesDefaultInitializer<Item>::value, Item>::isNull(it);
        }
    };

    template<typename Item>
    class SVGList : public RefCounted<SVGList<Item> > {
    private:
        typedef SVGListTypeOperations<Item> TypeOperations;

    public:
        virtual ~SVGList() { }

        const QualifiedName& associatedAttributeName() const { return m_associatedAttributeName; }

        unsigned int numberOfItems() const { return m_vector.size(); }
        void clear(ExceptionCode &) { m_vector.clear(); }

        Item initialize(Item newItem, ExceptionCode& ec)
        {
            if (TypeOperations::isNull(newItem)) {
                ec = TYPE_MISMATCH_ERR;
                return TypeOperations::nullItem();
            }
            clear(ec);
            return appendItem(newItem, ec);
        }

        Item getFirst() const
        {
            ExceptionCode ec = 0;
            return getItem(0, ec);
        }

        Item getLast() const
        {
            ExceptionCode ec = 0;
            return getItem(m_vector.size() - 1, ec);
        }

        Item getItem(unsigned int index, ExceptionCode& ec)
        {
            if (index >= m_vector.size()) {
                ec = INDEX_SIZE_ERR;
                return TypeOperations::nullItem();
            }

            return m_vector.at(index);
        }

        const Item getItem(unsigned int index, ExceptionCode& ec) const
        {
            if (index >= m_vector.size()) {
                ec = INDEX_SIZE_ERR;
                return TypeOperations::nullItem();
            }

            return m_vector[index];
        }

        Item insertItemBefore(Item newItem, unsigned int index, ExceptionCode& ec)
        {
            if (TypeOperations::isNull(newItem)) {
                ec = TYPE_MISMATCH_ERR;
                return TypeOperations::nullItem();
            }

            if (index < m_vector.size()) {
                m_vector.insert(index, newItem);
            } else {
                m_vector.append(newItem);
            }
            return newItem;
        }

        Item replaceItem(Item newItem, unsigned int index, ExceptionCode& ec)
        {
            if (index >= m_vector.size()) {
                ec = INDEX_SIZE_ERR;
                return TypeOperations::nullItem();
            }
    
            if (TypeOperations::isNull(newItem)) {
                ec = TYPE_MISMATCH_ERR;
                return TypeOperations::nullItem();
            }

            m_vector[index] = newItem;
            return newItem;
        }

        Item removeItem(unsigned int index, ExceptionCode& ec)
        {
            if (index >= m_vector.size()) {
                ec = INDEX_SIZE_ERR;
                return TypeOperations::nullItem();
            }

            Item item = m_vector[index];
            m_vector.remove(index);
            return item;
        }

        Item appendItem(Item newItem, ExceptionCode& ec)
        {
            if (TypeOperations::isNull(newItem)) {
                ec = TYPE_MISMATCH_ERR;
                return TypeOperations::nullItem();
            }

            m_vector.append(newItem);
            return newItem;
        }

    protected:
        SVGList(const QualifiedName& attributeName) 
            : m_associatedAttributeName(attributeName)
        {
        }

    private:
        Vector<Item> m_vector;
        const QualifiedName& m_associatedAttributeName;
    };

    template<typename Item>
    class SVGPODListItem : public RefCounted<SVGPODListItem<Item> > {
    public:
        static PassRefPtr<SVGPODListItem> create() { return adoptRef(new SVGPODListItem); }
        static PassRefPtr<SVGPODListItem> copy(const Item& item) { return adoptRef(new SVGPODListItem(item)); }

        operator Item&() { return m_item; }
        operator const Item&() const { return m_item; }

        // Updating facilities, used by JSSVGPODTypeWrapperCreatorForList
        Item value() const { return m_item; }
        void setValue(const Item& newItem) { m_item = newItem; }

    private:
        SVGPODListItem() : m_item() { }
        SVGPODListItem(const Item& item) : RefCounted<SVGPODListItem<Item> >(), m_item(item) { }
        
        Item m_item;
    };

    template<typename Item>
    class SVGPODList : public SVGList<RefPtr<SVGPODListItem<Item> > > {
    public:
        Item initialize(Item newItem, ExceptionCode& ec)
        {
            SVGPODListItem<Item>* ptr(SVGList<RefPtr<SVGPODListItem<Item> > >::initialize(SVGPODListItem<Item>::copy(newItem), ec).get());
            if (!ptr)
                return Item();

            return static_cast<const Item&>(*ptr); 
        }

        Item getFirst() const
        {
            SVGPODListItem<Item>* ptr(SVGList<RefPtr<SVGPODListItem<Item> > >::getFirst().get());
            if (!ptr)
                return Item();

            return static_cast<const Item&>(*ptr);
        }

        Item getLast() const
        {
            SVGPODListItem<Item>* ptr(SVGList<RefPtr<SVGPODListItem<Item> > >::getLast().get());
            if (!ptr)
                return Item();

            return static_cast<const Item&>(*ptr); 
        }

        Item getItem(unsigned int index, ExceptionCode& ec)
        {
            SVGPODListItem<Item>* ptr(SVGList<RefPtr<SVGPODListItem<Item> > >::getItem(index, ec).get());
            if (!ptr)
                return Item();

            return static_cast<const Item&>(*ptr);
        }

        const Item getItem(unsigned int index, ExceptionCode& ec) const
        {
            SVGPODListItem<Item>* ptr(SVGList<RefPtr<SVGPODListItem<Item> > >::getItem(index, ec).get());
             if (!ptr)
                return Item();

            return static_cast<const Item&>(*ptr);
        }

        Item insertItemBefore(Item newItem, unsigned int index, ExceptionCode& ec)
        {
            SVGPODListItem<Item>* ptr(SVGList<RefPtr<SVGPODListItem<Item> > >::insertItemBefore(SVGPODListItem<Item>::copy(newItem), index, ec).get());
            if (!ptr)
                return Item();

            return static_cast<const Item&>(*ptr); 
        }

        Item replaceItem(Item newItem, unsigned int index, ExceptionCode& ec)
        {
            SVGPODListItem<Item>* ptr(SVGList<RefPtr<SVGPODListItem<Item> > >::replaceItem(SVGPODListItem<Item>::copy(newItem), index, ec).get());
            if (!ptr)
                return Item();

            return static_cast<const Item&>(*ptr);
        }

        Item removeItem(unsigned int index, ExceptionCode& ec)
        {
            SVGPODListItem<Item>* ptr(SVGList<RefPtr<SVGPODListItem<Item> > >::removeItem(index, ec).get());
            if (!ptr)
                return Item();

            return static_cast<const Item&>(*ptr); 
        }

        Item appendItem(Item newItem, ExceptionCode& ec)
        {
            SVGPODListItem<Item>* ptr(SVGList<RefPtr<SVGPODListItem<Item> > >::appendItem(SVGPODListItem<Item>::copy(newItem), ec).get());
            if (!ptr)
                return Item();

            return static_cast<const Item&>(*ptr); 
        }
        
    protected:
        SVGPODList(const QualifiedName& attributeName) 
            : SVGList<RefPtr<SVGPODListItem<Item> > >(attributeName) { }
    };

} // namespace WebCore

#endif // ENABLE(SVG)
#endif // SVGList_h
