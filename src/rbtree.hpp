////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Реализация классов красно-черного дерева
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      01.05.2017
///            This is a part of the course "Algorithms and Data Structures" 
///            provided by  the School of Software Engineering of the Faculty 
///            of Computer Science at the Higher School of Economics.
///
/// "Реализация" (шаблонов) методов, описанных в файле rbtree.h
///
////////////////////////////////////////////////////////////////////////////////

#include <stdexcept>        // std::invalid_argument

using namespace std;

namespace xi
{


//==============================================================================
// class RBTree::node
//==============================================================================

template<typename Element, typename Compar>
RBTree<Element, Compar>::Node::~Node()//Done
{
    if (_left)
        delete _left;
    if (_right)
        delete _right;
}


template<typename Element, typename Compar>
typename RBTree<Element, Compar>::Node *RBTree<Element, Compar>::Node::setLeft(Node *lf)//Done
{
    // предупреждаем повторное присвоение
    if (_left == lf)
        return nullptr;

    // если новый левый — действительный элемент
    if (lf)
    {
        // если у него был родитель
        if (lf->_parent)
        {
            // ищем у родителя, кем был этот элемент, и вместо него ставим бублик
            if (lf->_parent->_left == lf)
                lf->_parent->_left = nullptr;
            else                                    // доп. не проверяем, что он был правым, иначе нарушение целостности
                lf->_parent->_right = nullptr;
        }

        // задаем нового родителя
        lf->_parent = this;
    }

    // если у текущего уже был один левый — отменяем его родительскую связь и вернем его
    Node *prevLeft = _left;
    _left = lf;

    if (prevLeft)
        prevLeft->_parent = nullptr;

    return prevLeft;
}


template<typename Element, typename Compar>
typename RBTree<Element, Compar>::Node *RBTree<Element, Compar>::Node::setRight(Node *rg)//Done
{
    // предупреждаем повторное присвоение
    if (_right == rg)
        return nullptr;

    // если новый правый — действительный элемент
    if (rg)
    {
        // если у него был родитель
        if (rg->_parent)
        {
            // ищем у родителя, кем был этот элемент, и вместо него ставим бублик
            if (rg->_parent->_left == rg)
                rg->_parent->_left = nullptr;
            else                                    // доп. не проверяем, что он был правым, иначе нарушение целостности
                rg->_parent->_right = nullptr;
        }

        // задаем нового родителя
        rg->_parent = this;
    }

    // если у текущего уже был один левый — отменяем его родительскую связь и вернем его
    Node *prevRight = _right;
    _right = rg;

    if (prevRight)
        prevRight->_parent = nullptr;

    return prevRight;
}


//==============================================================================
// class RBTree
//==============================================================================

template<typename Element, typename Compar>
RBTree<Element, Compar>::RBTree()
{
    _root = nullptr;
    _dumper = nullptr;
}

template<typename Element, typename Compar>
RBTree<Element, Compar>::~RBTree()
{
    // грохаем пока что всех через корень
    if (_root)
        delete _root;
}


template<typename Element, typename Compar>
void RBTree<Element, Compar>::deleteNode(Node *nd)//Done
{
    // если переданный узел не существует, просто ничего не делаем, т.к. в вызывающем проверок нет
    if (nd == nullptr)
        return;

    // потомков убьет в деструкторе
    delete nd;
}


template<typename Element, typename Compar>
void RBTree<Element, Compar>::insert(const Element &key)//Done
{
    // этот метод можно оставить студентам целиком
    Node *newNode = insertNewBstEl(key);

    // отладочное событие
    if (_dumper)
        _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_BST_INS, this, newNode);

    rebalance(newNode);

    // отладочное событие
    if (_dumper)
        _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_INSERT, this, newNode);

}


template<typename Element, typename Compar>
const typename RBTree<Element, Compar>::Node *RBTree<Element, Compar>::find(const Element &key)
{
    Node *startNode = _root; //начинаем искать с корня
    while (startNode != nullptr && (key < startNode->_key || startNode->_key < key))
    {
        if (key < startNode->_key) // если ключ меньше, то идем налево, если больше, то направо
            startNode = startNode->_left;
        else
            startNode = startNode->_right;
    }
    return startNode;
}

template<typename Element, typename Compar>
typename RBTree<Element, Compar>::Node *
RBTree<Element, Compar>::insertNewBstEl(const Element &key)
{
    Node *in = new Node(key, nullptr, nullptr, nullptr, RED);//создаем новую запись по ключу и изначально красим  в красный
    Node *futureparent = nullptr;//родитель
    Node *Letssearchit = _root; //поисковой элемнт
    while (Letssearchit != nullptr) // пока поиск не уперся в ноль, назначаем родителем поисковой элеемт и идем по ключам
    {
        futureparent = Letssearchit;
        if (key > Letssearchit->_key) // если больше, то вправо, если меньше, то велево
            Letssearchit = Letssearchit->_right;
        else
            Letssearchit = Letssearchit->_left;
    }
    in->_parent = futureparent;//назначаем родителем
    if (futureparent == nullptr) //если родителя нет, значит мы вставляем корень
        _root = in;

    else if (key < futureparent->_key)
        futureparent->_left = in;
    else
        futureparent->_right = in;

    return in;
}


template<typename Element, typename Compar>
typename RBTree<Element, Compar>::Node *
RBTree<Element, Compar>::rebalanceDUG(Node *nd)
{
    // TODO: этот метод студенты могут оставить и реализовать при декомпозиции балансировки дерева
    // В методе оставлены некоторые важные комментарии/snippet-ы


    // попадание в этот метод уже означает, что папа есть (а вот про дедушку пока не известно)
    //...

    //cоздаем две переменных для более удобного общения
    Node *papa = nd->_parent;
    Node *ded = papa->_parent;
    Node *uncle; // для левого случая нужен правый дядя и наоборот.
    if (!ded) //если не существует деда, то нет и дяди
        uncle = nullptr;
    else
    {//для левого случаея - правый и наоборот
        if (papa->isRightChild())
            uncle = ded->_left;
        else
            uncle = ded->_right;
    }

    // если дядя такой же красный, как сам нод и его папа...
    if (uncle != nullptr && uncle->isRed())
    {
        // дядю и папу красим в черное
        uncle->_color = BLACK;
        papa->_color = BLACK;
        // а дедушку — в коммунистические цвета
        ded->_color = RED;
        // отладочное событие
        if (_dumper)
            _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_RECOLOR1, this, nd);

        // теперь чередование цветов "узел-папа-дедушка-дядя" — К-Ч-К-Ч, но надо разобраться, что там
        // с дедушкой и его предками, поэтому продолжим с дедушкой
        nd = ded;
    } else
    {
        if (papa->isLeftChild() && nd->isRightChild())
        {
            nd = papa;
            rotLeft(nd);
        } else if (papa->isRightChild() && nd->isLeftChild())
        {
            nd = papa;
            rotRight(nd);
        }
    }
    papa->_color = BLACK;


    // отладочное событие
    if (_dumper)
        _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_RECOLOR3D, this, nd);


    ded->_color = RED;
    if (papa->isLeftChild())
        rotRight(ded);
    else
        rotLeft(ded);
    // отладочное событие
    if (_dumper)
        _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_RECOLOR3G, this, nd);


    return nd;
}


template<typename Element, typename Compar>
void RBTree<Element, Compar>::rebalance(Node *nd)
{
    //сначала проверим на наличие родителя и защитимся от утечки памяти, если родителя нет, то очевидно, что в дереве есть только корень, он черный, lets do it
    // интересно, этих комментов хватит на бонус?( у меня и так ср по нулям, хочу бонус(((( первый раз в жизни по алгосам комменты пишу
    if (nd->_parent == nullptr)
    {
        nd->_color = BLACK;
        _root = nd;
        return;
    }
    //если цвет родителя уже черный,все и так гуд
    if (nd->_parent->_color == BLACK)
        return;

    rebalanceDUG(nd);

}


template<typename Element, typename Compar>
void RBTree<Element, Compar>::rotLeft(typename RBTree<Element, Compar>::Node *nd)
{
    // TODO: метод реализуют студенты
    if (nd == nullptr)
        return;
//харним правый элемент от текущего
    Node *rightelement = nd->_right;
//не теряем память
    if (!rightelement)
        throw invalid_argument("rotLeft !rightelement error");
//переперисваеваем элементы
    nd->_right = rightelement->_left;
    //если левый элемент не конечный, то назначаем его родителем текущий
    if (rightelement->_left)
        rightelement->_left->_parent = nd;
    //если сам элемент не конечный, то назначаем его родителем родитяля текущего
    if (rightelement)
        rightelement->_parent = nd->_parent;
    //если родитель существует то перепресваиваем
    if (nd->_parent)
    {
        if (nd == nd->_parent->_left)
            nd->_parent->_left = rightelement;
        else
            nd->_parent->_right = rightelement;
    } else //если родителя не было, то корень - и есть элемент
    {
        _root = rightelement;
    }

    rightelement->_left = nd;
    if (nd != nullptr)
        nd->_parent = rightelement;


    if (_dumper)
        _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_LROT, this, nd);
}


template<typename Element, typename Compar>
void RBTree<Element, Compar>::rotRight(typename RBTree<Element, Compar>::Node *nd)
{
    // TODO: метод реализуют студенты

    //комментарии аналогичны левому повороту
    if (nd == nullptr)
        return;

    Node *leftelement = nd->_left;

    if (!leftelement)
        throw invalid_argument("rotRight !leftelemnt error");


    nd->_left = leftelement->_right;
    if (leftelement->_right != nullptr)
        leftelement->_right->_parent = nd;

    if (leftelement != nullptr)
        leftelement->_parent = nd->_parent;
    if (nd->_parent)
    {
        if (nd == nd->_parent->_right)
            nd->_parent->_right = leftelement;
        else
            nd->_parent->_left = leftelement;
    } else
    {
        _root = leftelement;
    }

    leftelement->_right = nd;
    if (nd != nullptr)
        nd->_parent = leftelement;


    // отладочное событие
    if (_dumper)
        _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_RROT, this, nd);
}

template<typename Element, typename Compar>
void RBTree<Element, Compar>::remove(const Element &key)
{

    //я пытался
    Node *first, *second;

    Node *search = _root;

    while (search != nullptr && (key < search->_key || search->_key < key))
    {
        if (key < search->_key)
            search = search->_left;
        else
            search = search->_right;
    }
    if (search == nullptr)
        return;

    if (search->_left == nullptr || search->_right == nullptr)
    {
        second = search;
    } else
    {
        second = search->_right;
        while (second->_left != nullptr)
            second = second->_left;
    }


    if (second->_left != nullptr)
        first = second->_left;
    else
        first = second->_right;

    first->_parent = second->_parent;
    if (second->_parent)
        if (second == second->_parent->_left)
            second->_parent->_left = first;
        else
            second->_parent->_right = first;
    else
        _root = first;

    if (second != search)
        search->_key = second->_key;


    if (second->_color == BLACK)
        rebalance(first);

    delete second;
}


} // namespace xi

