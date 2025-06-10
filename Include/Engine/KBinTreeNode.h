/* ��������������Ҳ�����ڴ�����ͨ�������
 * QA Pass
 */

#ifndef _ENGINE_TREENODE_H_
#define _ENGINE_TREENODE_H_

template <class T>
class BinSTree;

template <class T>
class TreeNode
{
   protected:
      // ָ�������Һ��ӵ�ָ��
      TreeNode<T> *left;
      TreeNode<T> *right;

   public:
      // ���г�Ա�������ⲿ�޸ĵ�����ֵ
      T data;
   
      // ���캯��
	  TreeNode (const T& item, TreeNode<T> *lptr = NULL,
                TreeNode<T> *rptr = NULL);
   
      // ����ָ����ĺ���
      TreeNode<T>* Left() const;
      TreeNode<T>* Right() const;

      friend class BinSTree<T>;
};

// ���캯������ʼ���������ݺ�ָ���򣬶��ڿ�����������ָ����ֵΪ NULL
template <class T>
TreeNode<T>::TreeNode (const T& item, TreeNode<T> *lptr,
    TreeNode<T> *rptr): data(item), left(lptr), right(rptr)
{}

// �����û���������
template <class T>
TreeNode<T>* TreeNode<T>::Left() const
{
   return left;
}

// �����û������Һ���
template <class T>
TreeNode<T>* TreeNode<T>::Right() const
{
   return right;
}

#endif // _ENGINE_TREENODE_H_
