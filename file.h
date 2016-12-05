
//Authors: Armante Washington & Ryan Nguyen
// file.h

#ifndef FILE_H
  #define	FILE_H

#include <iostream>
#include "Time.h"
#include "permissions.h"


class File
{
private:
  char *name;
  Time modTime;
  Permissions perms;
public:
  File (const char *nam, Permissions perm = Permissions(00666, NULL));
  File (const File &rhs);
  virtual ~File();
  virtual ostream& write (ostream &os)const;
  Permissions *permission;
  void changePerms(short perms); // permissions.chmod(perm)
  bool checkPerms(short perms, const char *usr)const;
  // return permissions.check(perms,usr)
  void chown(const char *usr);
  bool isFile()const;
  void print(bool var)const;
  void rename(const char *nam);
  const char* getName()const;
  void touch();
  bool operator== (const File &rhs) const;
  bool operator< (const File &rhs) const;
  friend void operator<< (ostream &os, File &file);
  friend istream& operator>> (istream &is, File &file);
  
  
  
};


#endif	/* FILE_H */


