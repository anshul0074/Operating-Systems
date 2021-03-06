#include <stdio.h>
#include <stdlib.h>

struct node {
  int key;
  struct node *left, *right;
};
struct node *newNode(int val) {
  struct node *root=(struct node*)malloc(sizeof(struct node));
  root->key=val;
  root->left=NULL;
  root->right=NULL;
  return root;
}
void inorder(struct node *root) {
	if(root==NULL){
		return;
	}
    inorder(root->left);
    printf("%d ",root->key);
    inorder(root->right);
}
struct node *insert(struct node *node,int key){
  if(node==NULL){
	return newNode(key);
  }
  if(key<node->key){
    node->left=insert(node->left,key);
  }
  else if(key>node->key){
    node->right=insert(node->right,key);
  }
  return node;
}
struct node *inordersuccesor(struct node *node) {
  struct node *temp=node;
  while(temp && temp->left!=NULL){
    temp=temp->left;
  }
  return temp;
}
struct node *erase(struct node *root,int key){
	if(root==NULL){ 
		return root;
	}
  	if(key<root->key){
    	root->left=erase(root->left,key);
  	}
  	else if(key>root->key){
    	root->right=erase(root->right,key);
  	}
  	else{
    	if(root->left == NULL) {
      		struct node *temp=root->right;
      		free(root);
      		return temp;
    	}
		else if(root->right==NULL){
      		struct node *temp = root->left;
      		free(root);
      		return temp;
    	}
    	struct node *temp=inordersuccesor(root->right);
    	root->key=temp->key;
    	root->right=erase(root->right, temp->key);
  	}
  	return root;
}
int main(){
  struct node *root =NULL;
  root=insert(root,8);
  root=insert(root,3);
  root=insert(root,1);
  root=insert(root,6);
  root=insert(root,7);
  root=insert(root,10);
  root=insert(root,14);
  root=insert(root,4);
  inorder(root);
  printf("\n");
  root=erase(root,10);
  inorder(root);
}