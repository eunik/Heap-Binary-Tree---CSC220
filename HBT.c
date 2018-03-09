/* Eun Il Kim */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define BLOCKSIZE 256

typedef int object_t;
typedef int key_t;
typedef struct tr_n_t { key_t        key;
                    struct tr_n_t  *left;
                    struct tr_n_t *right;
                    int           height;
                    int           leaves;    //////////Modified
                      } tree_node_t;

tree_node_t *currentblock = NULL;
int    size_left;
tree_node_t *free_list = NULL;


tree_node_t *get_node()
{ tree_node_t *tmp;
  if( free_list != NULL )
  {  tmp = free_list;
     free_list = free_list -> left;
  }
  else
  {  if( currentblock == NULL || size_left == 0)
     {  currentblock =
                (tree_node_t *) malloc( BLOCKSIZE * sizeof(tree_node_t) );
        size_left = BLOCKSIZE;
     }
     tmp = currentblock++;
     size_left -= 1;
  }
  return( tmp );
}


void return_node(tree_node_t *node)
{  node->left = free_list;
   free_list = node;
}


tree_node_t *create_tree(void)
{  tree_node_t *tmp_node;
   tmp_node = get_node();
   tmp_node->left = NULL;
   return( tmp_node );
}

void left_rotation(tree_node_t *n)
{  tree_node_t *tmp_node;
   key_t        tmp_key;
   int          l_leaf,r_leaf;               ////////Modified
   tmp_node = n->left;
   tmp_key  = n->key;
   l_leaf=n->right->left->leaves;            ////////Modified
   r_leaf=n->right->right->leaves;           ////////Modified
   n->left  = n->right;
   n->key   = n->right->key;
   n->right = n->left->right;
   n->left->right = n->left->left;
   n->left->left  = tmp_node;
   n->left->key   = tmp_key;
   n->right->leaves=r_leaf;                  ////////Modified
   n->left->leaves=n->left->left->leaves+ l_leaf; ///Modified
}

void right_rotation(tree_node_t *n)
{  tree_node_t *tmp_node;
   key_t        tmp_key;
   int          l_leaf,r_leaf;               ////////Modified
   tmp_node = n->right;
   tmp_key  = n->key;
   l_leaf=n->left->left->leaves;             ////////Modified
   r_leaf=n->left->right->leaves;            ////////Modified
   n->right = n->left;
   n->key   = n->left->key;
   n->left  = n->right->left;
   n->right->left = n->right->right;
   n->right->right  = tmp_node;
   n->right->key   = tmp_key;
   n->left->leaves= l_leaf;                  ////////Modified
   n->right->leaves=n->right->right->leaves+r_leaf;//Modified
}


object_t *find(tree_node_t *tree, key_t query_key)
{  tree_node_t *tmp_node;
   if( tree->left == NULL )
     return(NULL);
   else
   {  tmp_node = tree;
      while( tmp_node->right != NULL )
      {   if( query_key < tmp_node->key )
               tmp_node = tmp_node->left;
          else
               tmp_node = tmp_node->right;
      }
      if( tmp_node->key == query_key )
         return( (object_t *) tmp_node->left );
      else
         return( NULL );
   }
}


///////////////////////MODIFIED//////////////////////////////
object_t *find_by_number(tree_node_t *tree, int k){
    if(tree->left==NULL || k<=0 || k>tree->leaves)
        return (NULL);
    tree_node_t *tmp_node=tree;
    while(tmp_node->right!=NULL){
        if(k<=tmp_node->left->leaves)
            tmp_node=tmp_node->left;
        else {
            k-=tmp_node->left->leaves;
            tmp_node=tmp_node->right;
        }
    }
    return ((object_t *) tmp_node->left);
}


int insert(tree_node_t *tree, key_t new_key, object_t *new_object)
//if empty we place a node with a left root and NULL right root
{  tree_node_t *tmp_node;
   int finished;
   if( tree->left == NULL )
   {  tree->left = (tree_node_t *) new_object;
      tree->key  = new_key;
      tree->height = 0;
      tree->right  = NULL;
      tree->leaves = 1;                     //////////Modified
   }
   else
     {  tree_node_t * path_stack[100]; int  path_st_p = 0;
      tmp_node = tree;
	  //We save the address of the left child node if the new 
		//node is less than the current. Otherwise it's right
		//child node
      while( tmp_node->right != NULL )
      {   path_stack[path_st_p++] = tmp_node;
          if( new_key < tmp_node->key )
               tmp_node = tmp_node->left;
          else
               tmp_node = tmp_node->right;
      }
      /* found the candidate leaf. Test whether key distinct */
      if( tmp_node->key == new_key )
         return( -1 );
      /* key is distinct, now perform the insert */
	  //Saves the current node
	  //And sets up a new node with left tree_node obj and right NULL
      {  tree_node_t *old_leaf, *new_leaf;
         old_leaf = get_node();
         old_leaf->left = tmp_node->left;
         old_leaf->key = tmp_node->key;
         old_leaf->right  = NULL;
         old_leaf->height = 0;
         old_leaf->leaves=1;                ///////////Modified
         new_leaf = get_node();
         new_leaf->left = (tree_node_t *) new_object;
         new_leaf->key = new_key;
         new_leaf->right = NULL;
         new_leaf->height = 0;
         new_leaf->leaves=1;                ///////////Modified
		 //Places the new node on the right if greater than current node
         if( tmp_node->key < new_key )
         {   tmp_node->left  = old_leaf;
             tmp_node->right = new_leaf;
             tmp_node->key = new_key;
         }//Otherwise to the left
         else
         {   tmp_node->left  = new_leaf;
             tmp_node->right = old_leaf;
         }
         tmp_node->height = 1;
         tmp_node->leaves=2;                ///////////Modified
      }
      /* rebalance */
      finished = 0;
      while( path_st_p > 0 && !finished )
      {  int tmp_height, old_height;
         tmp_node = path_stack[--path_st_p];
         tmp_node->leaves++;                ///////////Modified
         old_height= tmp_node->height;
         if( tmp_node->left->height -
                                 tmp_node->right->height == 2 )
         {  if( tmp_node->left->left->height -
                                 tmp_node->right->height == 1 )
            {  right_rotation( tmp_node );
               tmp_node->right->height =
                            tmp_node->right->left->height + 1;
               tmp_node->height = tmp_node->right->height + 1;
            }
            else
            {  left_rotation( tmp_node->left );
               right_rotation( tmp_node );
               tmp_height = tmp_node->left->left->height;
               tmp_node->left->height  = tmp_height + 1;
               tmp_node->right->height = tmp_height + 1;
               tmp_node->height = tmp_height + 2;
            }
         }
         else if ( tmp_node->left->height -
                                tmp_node->right->height == -2 )
         {  if( tmp_node->right->right->height -
                                  tmp_node->left->height == 1 )
            {  left_rotation( tmp_node );
               tmp_node->left->height =
                           tmp_node->left->right->height + 1;
               tmp_node->height = tmp_node->left->height + 1;
            }
            else
            {  right_rotation( tmp_node->right );
               left_rotation( tmp_node );
               tmp_height = tmp_node->right->right->height;
               tmp_node->left->height  = tmp_height + 1;
               tmp_node->right->height = tmp_height + 1;
               tmp_node->height = tmp_height + 2;
            }
         }
         else /* update height even if there was no rotation */
         {  if( tmp_node->left->height > tmp_node->right->height )
               tmp_node->height = tmp_node->left->height + 1;
            else
               tmp_node->height = tmp_node->right->height + 1;
         }
         if( tmp_node->height == old_height )
            finished = 1;
      }
       while(path_st_p>0)                 ///////////Modified
           path_stack[--path_st_p]->leaves++; ///////Modified
   }
   return( 0 );
}



object_t *delete(tree_node_t *tree, key_t delete_key)
{  tree_node_t *tmp_node, *upper_node, *other_node;
   object_t *deleted_object; int finished;
   if( tree->left == NULL )
      return( NULL );
   else if( tree->right == NULL )
   {  if(  tree->key == delete_key )
      {  deleted_object = (object_t *) tree->left;
         tree->left = NULL;
         tree->leaves--;
         return( deleted_object );
      }
      else
         return( NULL );
   }
   else
   {  tree_node_t * path_stack[100]; int path_st_p = 0;
      tmp_node = tree;
      while( tmp_node->right != NULL )
      {   path_stack[path_st_p++] = tmp_node;
          upper_node = tmp_node;
          if( delete_key < tmp_node->key )
          {  tmp_node   = upper_node->left;
             other_node = upper_node->right;
          }
          else
          {  tmp_node   = upper_node->right;
             other_node = upper_node->left;
          }
      }
      if( tmp_node->key != delete_key )
         deleted_object = NULL;
      else
      {  upper_node->key   = other_node->key;
         upper_node->left  = other_node->left;
         upper_node->right = other_node->right;
         upper_node->height = other_node->height;
         upper_node->leaves = other_node->leaves; /////Modified
         deleted_object = (object_t *) tmp_node->left;
         return_node( tmp_node );
         return_node( other_node );

      }
      /*start rebalance*/
      finished = 0; path_st_p -= 1;
      while( path_st_p > 0 && !finished )
      {  int tmp_height, old_height;
         tmp_node = path_stack[--path_st_p];
         tmp_node->leaves--;                ///////////Modified
         old_height= tmp_node->height;
         if( tmp_node->left->height -
                                 tmp_node->right->height == 2 )
         {  if( tmp_node->left->left->height -
                                 tmp_node->right->height == 1 )
        {  right_rotation( tmp_node );
               tmp_node->right->height =
                            tmp_node->right->left->height + 1;
               tmp_node->height = tmp_node->right->height + 1;
            }
            else
        {  left_rotation( tmp_node->left );
               right_rotation( tmp_node );
               tmp_height = tmp_node->left->left->height;
               tmp_node->left->height  = tmp_height + 1;
               tmp_node->right->height = tmp_height + 1;
               tmp_node->height = tmp_height + 2;
            }
         }
         else if ( tmp_node->left->height -
                                tmp_node->right->height == -2 )
         {  if( tmp_node->right->right->height -
                                  tmp_node->left->height == 1 )
        {  left_rotation( tmp_node );
               tmp_node->left->height =
                           tmp_node->left->right->height + 1;
               tmp_node->height = tmp_node->left->height + 1;
            }
            else
            {  right_rotation( tmp_node->right );
               left_rotation( tmp_node );
               tmp_height = tmp_node->right->right->height;
               tmp_node->left->height  = tmp_height + 1;
               tmp_node->right->height = tmp_height + 1;
               tmp_node->height = tmp_height + 2;
            }
         }
         else /* update height even if there was no rotation */
         {  if( tmp_node->left->height > tmp_node->right->height )
               tmp_node->height = tmp_node->left->height + 1;
            else
               tmp_node->height = tmp_node->right->height + 1;
         }
         if( tmp_node->height == old_height )
             finished = 1;
      }
       while(path_st_p>0)                 ///////////Modified
           path_stack[--path_st_p]->leaves--; ///////Modified
      /*end rebalance*/
      return( deleted_object );
   }
}


void check_tree( tree_node_t *tr, int depth, int lower, int upper )
{  if( tr->left == NULL )
   {  printf("Tree Empty\n"); return; }
   if( tr->key < lower || tr->key >= upper )
         printf("Wrong Key Order \n");
   if( tr->right == NULL )
   {  if( *( (int *) tr->left) == 10*tr->key + 2 )
         printf("%d(%d)  ", tr->key, depth);
      else
         printf("Wrong Object \n");
   }
   else
   {  check_tree(tr->left, depth+1, lower, tr->key );
      check_tree(tr->right, depth+1, tr->key, upper );
   }
}

int main()
{  tree_node_t *searchtree;
   char nextop;
   searchtree = create_tree();
   printf("Made Tree: Height-Balanced Tree\n");
   while( (nextop = getchar())!= 'q' )
   { // Insert new node
	if( nextop == 'i' )
     { int inskey,  *insobj, success;
       insobj = (int *) malloc(sizeof(int));
       scanf(" %d", &inskey);
	   if(inskey > -1000 && inskey < 1000){
		   *insobj = 10*inskey+2;
		   success = insert( searchtree, inskey, insobj );
		   if ( success == 0 )
			  printf("  insert successful, key = %d, object value = %d, "
					 "height is %d, and leaves is %d\n",       /////////Modified
				  inskey, *insobj, searchtree->height,searchtree->leaves );
		   else
			   printf("  insert failed, success = %d\n", success);
	   }
	   else
	 printf("Must have |value| < 1000\n");
     }
	 // Finds nodes
     if( nextop == 'f' )
     { int findkey, *findobj;
       scanf(" %d", &findkey);
       findobj = find( searchtree, findkey);
       if( findobj == NULL )
         printf("  find failed, for key %d\n", findkey);
       else
         printf("  find successful, found object %d\n", *findobj);
     }
   //////////////////MODIFIED/////////////////////
   // Finds the leaves
     if( nextop == 'l' )
     { int findkey, *findobj;
       scanf(" %d", &findkey);
       findobj = find_by_number( searchtree, findkey);
       if( findobj == NULL )
         printf("  find failed, for leaves %d\n", findkey);
       else
         printf("  find successful, found object %d\n", *findobj);
     }
	// Deletes tree node
     if( nextop == 'd' )
     { int delkey, *delobj;
       scanf(" %d", &delkey);
       delobj = delete( searchtree, delkey);
       if( delobj == NULL )
         printf("  delete failed for key %d\n", delkey);
       else
         printf("  delete successful, deleted object %d, height is now, %d"
                 " and leaves is now %d\n",  /////////Modified
                 *delobj, searchtree->height,searchtree->leaves);
     }
	// Prints the tree 
     if( nextop == '?' )
     {  printf("  Checking tree\n");
        check_tree(searchtree,0,-1000,1000);
        printf("\n");
        if( searchtree->left != NULL )
      printf("key in root is %d, height of tree is %d, and leaves of tree"
              " is now %d\n",               /////////////Modified
         searchtree->key, searchtree->height, searchtree->leaves);
        printf("  Finished Checking tree\n");
     }
   }
   return(0);
}
