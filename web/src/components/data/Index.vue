<template>
  <div>
    <el-table :data="list" stripe :height="tableHeight">
      <el-table-column
        prop="id"
        label="ID">
      </el-table-column>

      <el-table-column
        prop="url"
        label="URL">
      </el-table-column>

      <el-table-column
        prop="title"
        label="标题">
      </el-table-column>

      <el-table-column
        fixed="right"
        label="操作">
        <template slot-scope="scope">
          <el-button type="text" size="small" @click="$router.push({ name: 'Data.Detail', params: { id: scope.row.id } })">详情</el-button>
        </template>
      </el-table-column>             
    </el-table>

    <el-pagination
      class="pagination"
      layout="prev, pager, next"
      ref="pagination"
      :page-count="pageCount"
      :current-page="currentPage"
      @current-change="loadData">
    </el-pagination>
  </div>
</template>

<script>
import Vue from 'vue'
import { mapActions } from 'vuex'
import { Loading } from 'element-ui'

export default {
  data () {
    return {
      tableHeight: 0,

      list: [],
      currentPage: 1,
      pageCount: 0
    }
  },

  methods: {
    ...mapActions(['getData']),

    loadData (page) {
      let params = {
        page
      }
      let loading = Loading.service()
      this.getData(params).then(resp => {
        const data = resp.data
        this.list = data.datas
        this.pageCount = data.pageCount
        this.currentPage = data.currentPage
      }).finally(() => loading.close())
    }
  },

  mounted () {
    // Here we must use nextTick, otherwise the effect may be overriden
    Vue.nextTick(() => {
      this.tableHeight = window.innerHeight - this.$refs.pagination.$el.clientHeight
    })
    this.loadData(this.currentPage)
  }
}
</script>

<style type="text/css">
  .pagination {
    padding: 50px 0;
    text-align: center;
  }
</style>
