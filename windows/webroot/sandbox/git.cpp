#include <sys/stat.h>
#include <string>
#include <fstream>
#include <iostream>
#include <git2.h>
using namespace std;


int main (int argc, char** argv) // Todo
{
  git_threads_init ();

  // Create repository directory.
  string directory = "repository";
  mkdir (directory.c_str(), 0777);

  // Initialize the repository: git init.
  git_repository *repo = NULL;
  int result = git_repository_init (&repo, directory.c_str(), false);
  if (result != 0) cerr << giterr_last ()->message << endl;

  // Store two files in the repository directory.
  ofstream file;
  file.open ("repository/foo", ios::binary | ios::trunc);
  file << "Contents of foo";
  file.close ();

  file.open ("repository/bar", ios::binary | ios::trunc);
  file << "Contents of bar";
  file.close ();

  // Get the git index.
  git_index * index = NULL;
  result = git_repository_index (&index, repo);
  if (result != 0) cerr << giterr_last ()->message << endl;

  // Add all files to the git index.
  result = git_index_add_all (index, NULL, 0, NULL, NULL);
  if (result != 0) cerr << giterr_last ()->message << endl;

  // Write the index to disk.
  result = git_index_write (index);
  if (result != 0) cerr << giterr_last ()->message << endl;

  // Create a tree from the index.
  git_oid tree_oid;
  result = git_index_write_tree (&tree_oid, index);
  if (result != 0) cerr << giterr_last ()->message << endl;
 
  git_signature * signature = NULL;
  result = git_signature_new (&signature, "username", "username@host", 1345678900, 0);
  if (result != 0) cerr << giterr_last ()->message << endl;
  
  git_tree *tree = NULL;
  result = git_tree_lookup (&tree, repo, &tree_oid);
  if (result != 0) cerr << giterr_last ()->message << endl;

  /* Get HEAD as a commit object to use as the parent of the commit */
  git_oid parent_id;
  git_commit *parent;
  git_reference_name_to_id (&parent_id, repo, "HEAD");
  git_commit_lookup(&parent, repo, &parent_id);
  
  git_buf buffer;
  memset (&buffer, 0, sizeof (git_buf));
  result = git_message_prettify (&buffer, "Initial commit", 0, '#');
  if (result != 0) cerr << giterr_last ()->message << endl;
  
  cout << "head unborn: " << git_repository_head_unborn (repo) << endl;
  
  git_oid commit_oid;
  result = git_commit_create_v (&commit_oid, repo, "HEAD", signature, signature, NULL, buffer.ptr, tree, 1, parent);
  if (result != 0) cerr << giterr_last ()->message << endl;

  cout << "head unborn: " << git_repository_head_unborn (repo) << endl;

  // Store one more file in the repository directory.
  file.open ("repository/baz", ios::binary | ios::trunc);
  file << "Contents of baz";
  file.close ();
  
  // Get the git index.
  result = git_repository_index (&index, repo);
  if (result != 0) cerr << giterr_last ()->message << endl;
  
  // Add all files to the git index.
  result = git_index_add_all (index, NULL, 0, NULL, NULL);
  if (result != 0) cerr << giterr_last ()->message << endl;
  
  // Write the index to disk.
  result = git_index_write (index);
  if (result != 0) cerr << giterr_last ()->message << endl;
  
  // Create a tree from the index.
  result = git_index_write_tree (&tree_oid, index);
  if (result != 0) cerr << giterr_last ()->message << endl;
  
  result = git_tree_lookup (&tree, repo, &tree_oid);
  if (result != 0) cerr << giterr_last ()->message << endl;
  
  memset (&buffer, 0, sizeof (git_buf));
  result = git_message_prettify (&buffer, "Subsequent commit", 0, '#');
  if (result != 0) cerr << giterr_last ()->message << endl;
  
  git_reference_name_to_id (&parent_id, repo, "HEAD");
  git_commit_lookup(&parent, repo, &parent_id);

  result = git_commit_create_v (&commit_oid, repo, "HEAD", signature, signature, NULL, buffer.ptr, tree, 1, parent);
  if (result != 0) cerr << giterr_last ()->message << endl;
  
  // Run "git status" to see the result.
  system ("cd repository; git status");

  // Free resources.
  git_buf_free (&buffer);
  git_tree_free (tree);
  git_signature_free (signature);
  git_index_free (index);
  git_repository_free (repo);
  git_threads_shutdown ();

  return 0;
}

