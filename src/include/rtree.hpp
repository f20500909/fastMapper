#ifndef SRC_INCLUDE_RTREE_HPP
#define SRC_INCLUDE_RTREE_HPP
#include <cmath>           //abs
#include <cassert>         //assert
#include <algorithm>        //swap
#include <functional>       //std::equal_to
#include <iterator>         //std::iterator
#include <memory>           //std::allocator
#include <limits>           //std::numeric_limits

#include <type_traits>
#include <new>
#include <vector>
#include <cstring>

using namespace std;

namespace rtree {
    class Point {
    public:
        typedef long coord_type;

        Point(long x, long y) : x_(x),
                                y_(y) {
        }


        long x() const {
            return x_;
        }

        long y() const {
            return y_;
        }

    private:
        long x_;
        long y_;

    };

    class Rect {
    public:
        Rect() : left_(numeric_limits<long>::max()),
                 right_(numeric_limits<long>::min()),
                 bottom_(numeric_limits<long>::max()),
                 top_(numeric_limits<long>::min()) {
        }


        Rect(long x1, long y1, long x2, long y2) : left_(min(x1, x2)),
                                                   right_(max(x1, x2)),
                                                   bottom_(min(y1, y2)),
                                                   top_(max(y1, y2)) {
        }


        long left() const {
            return left_;
        }


        long right() const {
            return right_;
        }


        long top() const {
            return top_;
        }


        long bottom() const {
            return bottom_;
        }

        //merge two rectangle into one bounding box
        void outersect(Rect const &oth) {
            left_ = min(left_, oth.left_);
            right_ = max(right_, oth.right_);
            bottom_ = min(bottom_, oth.bottom_);
            top_ = max(top_, oth.top_);

        }

        long width() const {
            return abs(right_ - left_);
        }

        long height() const {
            return abs(top_ - bottom_);
        }


    private:
        long left_;
        long bottom_;
        long right_;
        long top_;
    };


//return area of rect
    long area(Rect const &rect) {
        return rect.width() * rect.height();
    }

    long sqare_distance(Rect const &rect, Point const &point) {
        long dist = 0;

        long rect_coords[] = {rect.left(), rect.right(), rect.bottom(), rect.top()};
        long point_coords[] = {point.x(), point.y()};

        for (size_t ndim = 0; ndim < sizeof(point_coords) / sizeof(long); ++ndim) {
            long lbound = rect_coords[2 * ndim];
            long ubound = rect_coords[2 * ndim + 1];

            if (point_coords[ndim] < lbound)
                dist += (lbound - point_coords[ndim]) * (lbound - point_coords[ndim]);
            else if (point_coords[ndim] > ubound)
                dist += (ubound - point_coords[ndim]) * (ubound - point_coords[ndim]);
        }

        return dist;
    }

    void outersection(Rect const &lhs, Rect const &rhs, Rect &res) {
        res = lhs;
        res.outersect(rhs);
    }

    template<class T>
    struct lowest {
        static T value() {
            return std::numeric_limits<T>::min();
        }
    };

    template<class T, size_t F>
    struct tree_node {

        tree_node(size_t level = 0, Rect const &bbox = Rect())
                : level_(level), bbox_(bbox), count_(0), parent_(0), index_(0) {
        }


        ~tree_node() {
        }

        bool is_leaf() const {
            return 0 == level_;
        }

        bool empty() const {
            return 0 == count_;
        }

        size_t level() const {
            return level_;
        }

        size_t count() const {
            return count_;
        }

        size_t index() const {
            return index_;
        }

        tree_node *parent() const {
            return parent_;
        }

        Rect &bbox() {
            return bbox_;
        }

        Rect const &bbox() const {
            return bbox_;
        }

        //NOTE: data is constructed only for leaf nodes
        //that's why it can not be accessed in high level nodes
        T *data() {
            assert(level_ == 0);
            return static_cast<T *>(static_cast<void *>(&data_));
        }

        void clear() {
            //clear
            count_ = 0;
        }

        void reset_links(tree_node *parent, size_t index) {
            parent_ = parent;
            index_ = index;
        }


        //returns child branch
        tree_node *get_child(size_t i) {
            assert(i < count_);
            return childs_[i];
        }

        tree_node const *get_child(size_t i) const {
            assert(i < count_);
            return childs_[i];
        }

        //insert new child branch into the node and reset child/parent links
        bool add_child(tree_node *child) {
            if (count_ >= F) {
                return false;
            }

            //reset parent child links
            child->reset_links(this, count_);
            childs_[count_++] = child;
            return true;
        }


    private:
        size_t level_;         //level of the node
        size_t count_;         //first unused branch
        size_t index_;         //index of current branch in parent node (required for iterator implementation)
        tree_node *parent_;        //parent node (required for iterator implementation)
        Rect bbox_;          //bounding box
        tree_node *childs_[F];

        typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type data_;


    private:
        //disallow copy and assign
        tree_node(tree_node const &);

        tree_node &operator=(tree_node const &);
    };

    template<class T, size_t F>
    struct tree_iterator {
        typedef tree_node<typename std::remove_const<T>::type, F> tree_node;;

    public:
        tree_iterator(tree_node *root = nullptr) : node_(move_to_child(root)) {
        }

        tree_iterator(tree_iterator<typename std::remove_const<T>::type, F> const &oth) : node_(oth.node()) {
        }

        T &operator*() const {
            assert(node_ && node_->level() == 0);
            return *(node_->data());
        }

        T *operator->() const {
            assert(node_ && node_->level() == 0);
            return node_->data();
        }

        tree_iterator &operator++() {
            increment();
            return *this;
        }

        tree_iterator operator++(int) {
            tree_iterator temp(*this);
            ++*this;
            return temp;
        }

        void swap(tree_iterator &oth) {
            std::swap(node_, oth.node_);
        }

        bool operator==(tree_iterator const &oth) const {
            return node_ == oth.node_;
        }

        bool operator!=(tree_iterator const &oth) const {
            return !this->operator==(oth);
        }


        tree_node *node() const {
            return node_;
        }

    private:
        void increment() {
            if (!node_) {
                //end
                return;
            }

            node_ = move_to_parent(node_);
            node_ = move_to_child(node_);
        }

        //move current position to the first left leaf of the node
        tree_node *move_to_parent(tree_node *leaf) {
            //move to parent and skip all visited nodes
            for (tree_node *node = leaf; node && node->parent(); node = node->parent()) {
                if (node->index() + 1 < node->parent()->count())//check if all nodes in parent node are already visited
                {
                    //return next unvisited child
                    return node->parent()->get_child(node->index() + 1);
                }
            }

            //all nodes are already visited
            return nullptr;
        }

        //move current position to the first left leaf of the node
        tree_node *move_to_child(tree_node *node) {
            //move to first left leaf
            while (node && node->level() > 0) {
                node = node->empty() ? nullptr : node->get_child(0);
            }

            return node;
        }

    private:
        tree_node *node_;
    };


//struct for storing data found during nearest neighbour search
//and sorting branches by distance
    template<class T>
    struct heap_data {
        heap_data(Point::coord_type distance = 0, T const &data = T())
                : distance_(distance), data_(data) {}

        bool operator<(heap_data const &oth) {
            return distance_ < oth.distance_;
        }

        T data() const {
            return data_;
        }

        Point::coord_type distance() const {
            return distance_;
        }

    private:
        T data_;
        Point::coord_type distance_;
    };


    template<class T>
    class K_nearest_heap {
    public:
        K_nearest_heap(size_t max_size) : max_size_(max_size) {}

        bool full() const {
            return max_size_ <= data_.size();
        }

        bool empty() const {
            return data_.empty();
        }

        Point::coord_type max_distance() const {
            if (full()) {
                return data_.front().distance();
            }

            return std::numeric_limits<Point::coord_type>::max();
        }

        void pop() {
            std::pop_heap(data_.begin(), data_.end());
            data_.pop_back();
        }


        bool push(heap_data<T> const &item) {
            if (max_distance() < item.distance()) {
                //heap is full and current max distance is lower than item distance - skip current item
                return false;
            }

            if (full()) {
                pop();
            }

            data_.push_back(item);
            std::push_heap(data_.begin(), data_.end());
            return true;
        }

        heap_data<T> const &top() {
            return data_.front();
        }

    private:
        size_t max_size_;
        std::vector<heap_data<T> > data_;
    };

//R_tree implementation
    template<class T, size_t F = 2, class Alloc = std::allocator<T> >
    class R_tree {

    private:
        static const int kMaxFill = 2 * F + 1;


    public:
        typedef Point::coord_type coord_type;

        typedef tree_node<T, kMaxFill> tree_node;

        typedef tree_iterator<T, kMaxFill> iterator;
        typedef tree_iterator<const T, kMaxFill> const_iterator;

        //R_tree definition
        R_tree() : root_(nullptr) {
        }

        ~R_tree() {
            clean(root_);
        }

        iterator begin() {
            return iterator(root_);
        }

        const_iterator begin() const {
            return const_iterator(root_);
        }

        iterator end() {
            return iterator(nullptr);
        }

        const_iterator end() const {
            return const_iterator(nullptr);
        }

        void insert_internal(tree_node *leaf) {
            assert(leaf);
            assert(leaf->level() == 0);

            //选择一个pnode
            tree_node *pnode = nullptr;
            if (root_ && root_->level() > 0) {
                pnode = root_;//从根节点开始找
                while (pnode->level() > 1) {//遍历查找
                    size_t i = pick_branch(pnode, leaf->bbox());//选择一个分支
                    pnode = pnode->get_child(i);
                }
            }

            tree_node *split = leaf;
            while (pnode) {
                //update parent bbox
                //if pnode is being splitted then we recompute bbox in quadratic split
                //if it isn't then bbox has to be outersected with child bbox
                outersection(pnode->bbox(), leaf->bbox(), pnode->bbox()); //向外扩展box
                if (split) {
                    assert(validate_bbox(split));
                    split = add_branch(pnode, split); //add split node to parent and update parent bbox
                }

                assert(validate_bbox(pnode));
                pnode = pnode->parent();
            }
            if (split) {
                //root_ is nullptr or was splitted
                if (root_)
                    root_ = split_root(split);
                else {
                    assert(split == leaf);
                    root_ = split;
                }
                assert(validate_bbox(root_));
            }
        }

        tree_node *create_node(Rect const &bbox, T const &val) {
            tree_node *node = nalloc_.allocate(1);
            new(static_cast<void *>(node)) tree_node(0, bbox);//no throw

            new(static_cast<void *>(node->data())) T(val);

            return node;
        }

        //插入一个元素到rtree中
        void insert(Rect const &bbox, T const &data){
            tree_node *leaf = this->create_node(bbox, data);

            insert_internal(leaf);
        }

        //搜索一个点最近的几个点
        void nearest_search(Point const &query, size_t k, vector<T> &found) const {
            if (!root_)
                return;

            //数据储存在堆中
            K_nearest_heap<T> heap(k);

            //实际干活的函数
            nearest_search(root_, query, heap);

            //copy to output container
            while (!heap.empty()) {
                found.push_back(heap.top().data());
                heap.pop();
            }
        }

    private:

        void destroy_node(tree_node *node) {
            if (node->is_leaf())
                node->data()->~T();    //destroy value for leaf nodes

            node->~tree_node();
            nalloc_.deallocate(node, 1);
        }

        //used to create non-leaf nodes
        tree_node *create_node(size_t level) {
            assert(level > 0);

            tree_node *node = nalloc_.allocate(1);
            new(static_cast<void *>(node)) tree_node(level);//no throw
            return node;
        }



        static bool clean_all(tree_node *node) {
            return true;
        }

        void clean(tree_node *node, bool (*pred)(tree_node *) = clean_all) {
            if (!node || !pred(node)) {
                return;
            }

            for (size_t i = 0; i < node->count(); ++i) {
                clean(node->get_child(i), pred);
            }

            destroy_node(node);
        }

        void nearest_search(tree_node *node, Point const &query, K_nearest_heap<T> &heap) const {
            assert(node);

            if (node->is_leaf()) {
                coord_type current = sqare_distance(node->bbox(), query);
                heap.push(heap_data<T>(current, *(node->data())));
                return;
            }

            //sort bracnhes by distance to query point
            std::vector<heap_data<size_t> > sorted;
            for (size_t i = 0; i < node->count(); ++i) {
                coord_type distance = sqare_distance(node->get_child(i)->bbox(), query);
                sorted.push_back(heap_data<size_t>(distance, i));
            }
            std::sort(sorted.begin(), sorted.end());


            for (size_t i = 0; i < sorted.size(); ++i) {
                size_t current_branch = sorted[i].data();
                coord_type current_distance = sorted[i].distance();

                //check if heap contains k elemenents
                if (heap.max_distance() < current_distance) {
                    //do not search anymore since current distance is bigger than the worst distance in the heap
                    break;
                }

                //otherwise: search child nodes recursively
                nearest_search(node->get_child(current_branch), query, heap);
            }
        }


        tree_node *split_root(tree_node *split) {
            assert(root_ && split && split->level() == root_->level());

            tree_node *nroot;
            nroot = create_node(root_->level() + 1);

            nroot->add_child(root_);
            nroot->add_child(split);
            nroot->bbox() = recompute_bbox(nroot);
            return nroot;
        }

        Rect recompute_bbox(tree_node const *node) {
            assert(node && node->level() > 0);
            Rect bbox;
            for (size_t i = 0; i < node->count(); ++i)
                outersection(bbox, node->get_child(i)->bbox(), bbox);

            return bbox;
        }

        /*
        [Pick branch]
            Select branch which needs least enlargement to include new branch. When there are more
            qualify entries, the entry with the rectangle of the smallest area is chosen
        */
        size_t pick_branch(tree_node *node, Rect const &bbox) const {
            assert(node && node->count() > 0);

            size_t res = 0;
            coord_type min_area = 0;
            coord_type increase = std::numeric_limits<coord_type>::max();
            Rect merged;

            //遍历所有节点
            for (size_t i = 0; i < node->count(); ++i) {
                //计算其扩展的边界
                outersection(bbox, node->get_child(i)->bbox(), merged);
                //current branch area
                coord_type branch_area = area(node->get_child(i)->bbox());

                //required enlargement
                coord_type increase_area = area(merged) - branch_area;

                //choose branch with smallest enlargement and smallest area
                if (increase_area < increase) {
                    res = i;
                    increase = increase_area;
                    min_area = branch_area;
                } else if (increase_area == increase &&
                           branch_area < min_area) {
                    res = i;
                    min_area = branch_area;
                }
            }

            return res;
        }

        /*
        Quadratic Split pseudo-code:
            Divide a set of kMaxFill + 1 index entries into two groups.
            QS1 [Pick first entry for each group] start PickSeeds to find two entries to be
                the first elements of the groups. Assign each to a group.
            QS2 [Check if done] If all entries have been assigned, then break up. If a group
                has too few entries that all the rest must be assigned to it in order for it to
                have the minimum number m, then assign them and break up
        */
        void quadractic_split(tree_node *node, tree_node *split, tree_node *child) {
            assert(child && node && node->count() == kMaxFill);

            tree_node *branches[kMaxFill + 1];
            size_t groups[kMaxFill + 1];

            const size_t branches_size = node->count() + 1;

            branches[0] = child;
            for (size_t i = 0; i < node->count(); ++i) {
                branches[i + 1] = node->get_child(i);
            }

            size_t group_0 = 0;
            size_t group_1 = 0;

            //find groups
            pick_seeds(branches, branches_size, group_0, group_1);

            //split branches on two groups
            distribute_branches(branches, branches_size, groups, group_0, group_1);

            //assign nodes
            node->clear();
            for (size_t i = 0; i < branches_size; ++i) {
                if (groups[i] == group_0 + 1) {
                    node->add_child(branches[i]);
                } else {
                    split->add_child(branches[i]);
                }
            }
            node->bbox() = recompute_bbox(node);
            split->bbox() = recompute_bbox(split);
        }

        /*
        QS3 [Select entry to assign] start PickNext to choose the next entry to assign.
            This is put in the group whose area has to be least enlarged. If the algorithm
            enlarges both groups with the same size, then add to the group with smaller
            area, then to the one with fewer entries, then to either. Repeat from QS2.
        */
        void distribute_branches(tree_node **branches, size_t count, size_t *groups, size_t group_0, size_t group_1) {
            //clear groups
            memset(groups, 0, sizeof(size_t) * count);

            groups[group_0] = group_0 + 1;
            groups[group_1] = group_1 + 1;

            //number element in groups
            size_t nelems[] = {1, 1};
            size_t groupids[] = {group_0, group_1};
            Rect bboxes[] = {branches[group_0]->bbox(), branches[group_1]->bbox()};

            Rect merged;

            while (nelems[0] + nelems[1] < count &&
                   nelems[0] < count / 2 &&
                   nelems[1] < count / 2) {
                size_t best_branch = pick_next(branches, count, groups, bboxes[0], bboxes[1]);
                coord_type increase_0 = 0;
                coord_type increase_1 = 0;
                coord_type area_0 = area(bboxes[0]);
                coord_type area_1 = area(bboxes[1]);

                outersection(branches[best_branch]->bbox(), bboxes[0], merged);
                increase_0 = area(merged) - area_0;

                outersection(branches[best_branch]->bbox(), bboxes[1], merged);
                increase_1 = area(merged) - area_1;


                size_t group = 0;
                if (increase_0 != increase_1) {
                    group = increase_0 < increase_1 ? 0 : 1;
                } else if (area_0 != area_1) {
                    //the same increase
                    group = area_0 < area_1 ? 0 : 1;
                } else {
                    group = nelems[0] < nelems[1] ? 0 : 1;
                }

                ++nelems[group];
                groups[best_branch] = groupids[group] + 1;
                outersection(bboxes[group], branches[best_branch]->bbox(), bboxes[group]);
            }

            //distribute the rest of braches
            size_t group = nelems[0] < nelems[1] ? group_0 : group_1;
            for (size_t i = 0; i < count; ++i) {
                if (!groups[i])
                    groups[i] = group + 1;
            }
        }

        /*
        PickSeeds pseudo-code:
            PS1 [Calculate ineffiency of grouping entries together] For all pairs of entries
                E1 and E2 a rectangle J is created which includes E1:I and E2:I.
                Calculate
                d = area(J)  area(E1:I)  area(E2:I)
            PS2 [Choose the most wasterful pair] Choose the pair with the largest d
        */
        void pick_seeds(tree_node **branches, size_t count, size_t &group_0, size_t &group_1) {
            Rect merged;
            coord_type worst_area = lowest<coord_type>::value();

            for (size_t i = 0; i < count; ++i) {
                for (size_t j = i + 1; j < count; ++j) {
                    outersection(branches[i]->bbox(), branches[j]->bbox(), merged);
                    coord_type cur_area = area(merged) - area(branches[i]->bbox()) - area(branches[j]->bbox());

                    if (cur_area > worst_area) {
                        worst_area = cur_area;
                        group_0 = i;
                        group_1 = j;
                    }
                }
            }
        }


        /*
        PickNext pseudo-code:
            PN1 [Determine cost of putting each entry in each group] For each entry E which
                is not in a group yet, d1 is calculated. d1 is the area-increase required in
                the covering rectangle of group 1 to include E:I and also d2 for group 2.
            PN2 [Find entry with greatest preference for one
        */
        size_t pick_next(tree_node **branches, size_t count, size_t const *groups, Rect const &bbox_0, Rect const &bbox_1) {
            size_t res = 0;
            coord_type max_diff = lowest<coord_type>::value();

            Rect merged;
            for (size_t i = 0; i < count; ++i) {
                if (groups[i] > 0) {
                    //already picked
                    continue;
                }

                coord_type diff = 0;
                outersection(branches[i]->bbox(), bbox_0, merged);
                diff += area(merged) - area(bbox_0);

                outersection(branches[i]->bbox(), bbox_1, merged);
                diff -= area(merged) - area(bbox_1);

                if (max_diff < abs(diff)) {
                    max_diff = abs(diff);
                    res = i;
                }
            }

            return res;
        }

        //add child to the node and split node if node is overflowed
        tree_node *add_branch(tree_node *node, tree_node *child) {
            assert(node && child && node->level() == child->level() + 1);

            if (node->add_child(child)) {
                return nullptr;
            }

            tree_node *splitted;
            splitted = create_node(node->level());

            quadractic_split(node, splitted, child);
            return splitted;
        }

        bool validate_bbox(tree_node const *node) {
            if (!node || node->level() == 0)
                return true;

            Rect rect = recompute_bbox(node);

            return !(rect.left() != node->bbox().left() || rect.right() != node->bbox().right() ||
                     rect.top() != node->bbox().top() || rect.bottom() != node->bbox().bottom());

        }

    private:
        tree_node *root_;

        typename Alloc::template rebind<tree_node>::other nalloc_;

        R_tree(R_tree const &) = delete;

        R_tree &operator=(R_tree const &) = delete;
    };

}//namspace rtree_1


#endif //SRC_INCLUDE_RTREE_HPP
